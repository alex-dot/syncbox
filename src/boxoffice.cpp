/*
 * Boxoffice
 */

#include <zmqpp/zmqpp.hpp>
#include <boost/thread.hpp>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <endian.h>
#include <sys/inotify.h>
#include <sodium.h>

#include "file.hpp"
#include "constants.hpp"
#include "boxoffice.hpp"
#include "publisher.hpp"
#include "heartbeater.hpp"
#include "dispatcher.hpp"
#include "subscriber.hpp"

void *publisher_thread(zmqpp::context*, host_t host);
void *heartbeater_thread(zmqpp::context*, fsm::status_t status);
void *dispatcher_thread(zmqpp::context*, fsm::status_t status);
void *subscriber_thread(zmqpp::context*, node_t node);
void *box_thread(Box* box);

Boxoffice::~Boxoffice()
{
  // deleting threads
  for (std::vector<boost::thread*>::iterator i = pub_threads.begin(); i != pub_threads.end(); ++i)
    delete *i;
  for (std::vector<boost::thread*>::iterator i =  hb_threads.begin(); i !=  hb_threads.end(); ++i)
    delete *i;
  for (std::vector<boost::thread*>::iterator i = disp_threads.begin(); i != disp_threads.end(); ++i)
    delete *i;
  for (std::vector<boost::thread*>::iterator i = sub_threads.begin(); i != sub_threads.end(); ++i)
    delete *i;
  for (std::vector<boost::thread*>::iterator i = box_threads.begin(); i != box_threads.end(); ++i)
    delete *i;

  // deleting sockets
  delete z_bo_main;
  delete z_router;
  delete z_bo_pub;
  delete z_bo_hb;
  delete z_bo_disp;
  delete z_broadcast;
}

Boxoffice* Boxoffice::initialize(zmqpp::context* z_ctx)
{
  int return_value = 0;

  Boxoffice* bo = getInstance();

  Config* conf = Config::getInstance();
  bo->subscribers = conf->getNodes();
  bo->publishers = conf->getHosts();

  // setting up
  return_value = bo->setContext(z_ctx);
  if (return_value == 0) return_value = bo->connectToBroadcast();
  if (return_value == 0) return_value = bo->connectToMain();

  // setting up the subscribers and publishers, the router and aggregate the subs and pubs
  // note that the Watchers are all subscribers too!
  if (return_value == 0) return_value = bo->setupConnectionToChildren();
  if (return_value == 0) return_value = bo->setupBoxes();
  if (return_value == 0) return_value = bo->setupPublishers();
  if (return_value == 0) return_value = bo->setupHeartbeaters();
  if (return_value == 0) return_value = bo->setupDispatchers();
  if (return_value == 0) return_value = bo->setupSubscribers();
  if (return_value == 0) return_value = bo->checkChildren();
  if (return_value == 0) return_value = bo->runRouter();

  // closing
  bo->closeConnections();

  return bo;
}

int Boxoffice::setContext(zmqpp::context* z_ctx_)
{
  zmqpp::context* z_ctx_temp = z_ctx;
  z_ctx = z_ctx_;
      
  if ( z_ctx_temp == nullptr ) return 0;
  else return 1;

  return -1;
}

int Boxoffice::connectToBroadcast()
{
  // connect to process broadcast
  // since we want to rarely use that (mostly interrupt) we assume that by 
  // the time we need it, we have long been connected to it
  z_broadcast = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_broadcast->connect("inproc://sb_broadcast");
  z_broadcast->subscribe("");

  return 0;
}

int Boxoffice::connectToMain()
{
  // open PAIR sender to main thread
  z_bo_main = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_bo_main->connect("inproc://sb_bo_main_pair");

  return 0;
}

int Boxoffice::setupConnectionToChildren()
{
  // connection for information from subscribers, publishers and boxes
  z_router = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pull);
  z_router->bind("inproc://sb_boxoffice_pull_in");
  // connection to send information to publishers and boxes
  z_bo_pub = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pub);
  z_bo_pub->bind("inproc://sb_boxoffice_push_out");
  // connection to send information to heartbeaters
  z_bo_hb = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pub);
  z_bo_hb->bind("inproc://sb_boxoffice_hb_push_out");
  // connection to send information to dispatchers
  z_bo_disp = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pub);
  z_bo_disp->bind("inproc://sb_boxoffice_disp_push_out");
  if (SB_MSG_DEBUG) printf("bo: starting to listen to children...\n");

  return 0;
}

int Boxoffice::setupBoxes()
{
  Config* conf = Config::getInstance();
  std::vector< box_t > box_dirs = conf->getBoxes();

  box_threads.reserve(box_dirs.size());
  if (SB_MSG_DEBUG) printf("bo: opening %d box threads\n", (int)box_dirs.size());
  for (std::vector<box_t>::iterator i = box_dirs.begin(); i != box_dirs.end(); ++i)
  {
    // initializing the boxes here, so we can use file IO while it's thread 
    // still listens to inotify events
    Box* box = new Box(z_ctx, i->base_path, i->uid);
    Hash* hash = new Hash(i->uid);
    boxes.insert(std::make_pair(hash,box));

    // opening box thread
    boost::thread* bt = new boost::thread(box_thread, box);
    box_threads.push_back(bt);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d box threads\n", (int)box_dirs.size());

  return 0;
}

int Boxoffice::setupPublishers()
{
  // standard variables
  zmqpp::message z_msg;

  // opening publisher threads
  if (SB_MSG_DEBUG) printf("bo: opening %d publisher threads\n", (int)publishers.size());
  for (std::vector< host_t >::iterator i = publishers.begin(); 
        i != publishers.end(); ++i)
  {
    boost::thread* pub_thread = new boost::thread(publisher_thread, z_ctx, *i);
    pub_threads.push_back(pub_thread);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d publisher threads\n", (int)pub_threads.size());

  return 0;
}

int Boxoffice::setupHeartbeaters()
{
  // standard variables
  zmqpp::message z_msg;

  // opening heartbeater threads
  if (SB_MSG_DEBUG) printf("bo: opening %d heartbeater threads\n", (int)publishers.size());
  for (std::vector< host_t >::iterator i = publishers.begin(); 
        i != publishers.end(); ++i)
  {
    boost::thread* hb_thread = new boost::thread(heartbeater_thread, z_ctx, fsm::status_100);
    hb_threads.push_back(hb_thread);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d heartbeater threads\n", (int)hb_threads.size());

  return 0;
}

int Boxoffice::setupDispatchers()
{
  // standard variables
  zmqpp::message z_msg;

  // opening dispatcher threads
  if (SB_MSG_DEBUG) printf("bo: opening %d dispatcher threads\n", (int)publishers.size());
  for (std::vector< host_t >::iterator i = publishers.begin(); 
        i != publishers.end(); ++i)
  {
    boost::thread* disp_thread = new boost::thread(dispatcher_thread, z_ctx, fsm::status_100);
    disp_threads.push_back(disp_thread);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d dispatcher threads\n", (int)disp_threads.size());

  return 0;
}

int Boxoffice::setupSubscribers()
{
  // opening subscriber threads
  if (SB_MSG_DEBUG) printf("bo: opening %d subscriber threads\n", (int)subscribers.size());
  for (node_map::iterator i = subscribers.begin(); 
        i != subscribers.end(); ++i)
  {
    boost::thread* sub_thread = new boost::thread(subscriber_thread, 
                                                  z_ctx, 
                                                  i->second);
    sub_threads.push_back(sub_thread);
    ++total_node_number_;
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d subscriber threads\n", (int)sub_threads.size());

  return 0;
}

int Boxoffice::checkChildren() {
  // standard variables
  zmqpp::message z_msg;
  int msg_type, msg_signal;
  std::stringstream* sstream;

  // wait for heartbeat
  int heartbeats = sub_threads.size()
                 + pub_threads.size()
                 + hb_threads.size()
                 + disp_threads.size()
                 + box_threads.size();
  for (int i = 0; i < heartbeats; ++i)
  {
    sstream = new std::stringstream();
    s_recv(*z_router, *z_broadcast, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) return 1;
    delete sstream;
  }
  if (SB_MSG_DEBUG) printf("bo: all subscribers, publishers, heartbeaters, dispatchers and boxes connected\n");
  if (SB_MSG_DEBUG) printf("bo: counted %d threads\n", heartbeats);

  return 0;
}

int Boxoffice::closeConnections()
{
  // standard variables
  int msg_type, msg_signal;
  std::stringstream* sstream;
  int return_value = 0;

  // wait for exit/interrupt signal
  int heartbeats = sub_threads.size()
                 + pub_threads.size()
                 + hb_threads.size()
                 + disp_threads.size()
                 + box_threads.size();
  for (int i = 0; i < heartbeats; ++i)
  {
    sstream = new std::stringstream();
    s_recv(*z_router, *z_broadcast, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || (   msg_signal != SB_SIGLIFE_EXIT
                                         && msg_signal != SB_SIGLIFE_INTERRUPT ) ) return_value = 1;
    delete sstream;
  }
  if (SB_MSG_DEBUG) printf("bo: all subscribers, publishers, heartbeaters, dispatchers and boxes exited\n");

  // closing broadcast socket
  // we check if the sockets are nullptrs, but z_broadcast is never
  z_broadcast->close();

  // closing publisher, subscriber and box threads
  for (std::vector<boost::thread*>::iterator i = pub_threads.begin(); i != pub_threads.end(); ++i)
    (*i)->join();

  for (std::vector<boost::thread*>::iterator i =  hb_threads.begin(); i !=  hb_threads.end(); ++i)
    (*i)->join();

  for (std::vector<boost::thread*>::iterator i = disp_threads.begin(); i != disp_threads.end(); ++i)
    (*i)->join();

  for (std::vector<boost::thread*>::iterator i = sub_threads.begin(); i != sub_threads.end(); ++i)
    (*i)->join();

  for (std::vector<boost::thread*>::iterator i = box_threads.begin(); i != box_threads.end(); ++i)
    (*i)->join();

  if ( z_router != nullptr )
    z_router->close();

  z_bo_pub->close();
  z_bo_hb->close();
  z_bo_disp->close();

  // sending exit signal to the main thread...
  if (SB_MSG_DEBUG) printf("bo: sending exit signal...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_EXIT;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_bo_main->send(z_msg);
  // ...and exiting
  z_bo_main->close();

  if (SB_MSG_DEBUG) printf("bo: exit signal sent, exiting...\n");

  return return_value;
}

int Boxoffice::runRouter()
{ 
  // standard variables
  zmqpp::message z_msg;
  int msg_type, msg_signal;
  std::stringstream* sstream;

  if (SB_MSG_DEBUG) printf("bo: waiting for input from subscribers and watchers\n");
  while(true)
  {
    // waiting for subscriber or inotify input
    sstream = new std::stringstream();
    s_recv(*z_router, *z_bo_main, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type == SB_SIGTYPE_LIFE    && msg_signal == SB_SIGLIFE_INTERRUPT ) 
    {
      delete sstream;
      break;
    }

    int ret_val = processEvent((fsm::status_t)msg_signal, sstream);

    delete sstream;

    if (ret_val != 0) return ret_val;
  }

  return 0;
}

int Boxoffice::processEvent(fsm::status_t status, 
                            std::stringstream* sstream) {
  fsm::event_t event = fsm::get_event_by_status_code(status);

  if (SB_MSG_DEBUG) printf("bo: checking event with state %d, event %d and status %d\n", 
    state_, event, status);
  if ( fsm::check_event(state_, event, status) ) {

    // RECEIVED_HEARTBEAT_EVENT
    if ( event == fsm::received_heartbeat_event ) {
      // Synchronize the node's local time
      updateTimestamp(sstream);

      switch ( status ) {
        // STATUS_100
        // if the received status was simply 100, do nothing...
        case fsm::status_100: {
          break;
        }

        // STATUS_121
        // waiting for all nodes to reply, so increment node_reply_counter_
        // until all nodes replied, then manually change the status
        case fsm::status_121: {
          ++node_reply_counter_;

          if ( node_reply_counter_ == total_node_number_ ) {
            node_reply_counter_ = 0;
            status = fsm::status_122;
            event = fsm::get_event_by_status_code(status);
            if ( !check_event(state_, event, status) ) return 1;
          }

          break;
        }
        // STATUS_161
        // waiting for all nodes to reply, so increment node_reply_counter_
        // until all nodes replied, then manually change the status
        case fsm::status_161: {
          ++node_reply_counter_;

          if ( node_reply_counter_ == total_node_number_ ) {
            node_reply_counter_ = 0;
            status = fsm::status_162;
            event = fsm::get_event_by_status_code(status);
            if ( !check_event(state_, event, status) ) return 1;
          }

          break;
        }
        // STATUS_165
        // waiting for all nodes to reply, so increment node_reply_counter_
        // until all nodes replied, then manually change the status
        case fsm::status_165: {
          ++node_reply_counter_;

          if ( node_reply_counter_ == total_node_number_ ) {
            node_reply_counter_ = 0;
            status = fsm::status_166;
            event = fsm::get_event_by_status_code(status);
            if ( !check_event(state_, event, status) ) return 1;
          }

          break;
        }

        // STATUS_141
        // waiting for all nodes to reply, so increment node_reply_counter_
        // until all nodes replied, then manually change the status to 150
        case fsm::status_141: {
          ++node_reply_counter_;

          if ( node_reply_counter_ == total_node_number_ ) {
            node_reply_counter_ = -1;

            if (file_list_metadata_.size() > 0) {
              // have more metadata only files
              status = fsm::status_174;
            } else if (file_list_data_.size() > 1) {
              // have multiple files
              status = fsm::status_178;
            } else if (file_list_data_.size() > 0) {
              // have one file
              status = fsm::status_177;
            } else {
              // everything transmitted
              status = fsm::status_172;
            }
            event = fsm::get_event_by_status_code(status);
            if ( !check_event(state_, event, status) ) return 1;
          }

          break;
        }

        // STATUS_120 || STATUS_124 || STATUS_140
        case fsm::status_120:
        case fsm::status_124:
          {
            notified_dispatch_ = false;
            break;
          }

        // STATUS_130
        // waiting for file metadata
        case fsm::status_130: {
          if ( state_ == fsm::promoting_new_file_metadata_state
                && !notified_dispatch_ ) {
            sstream->seekg(1, std::ios_base::cur);
            char box_hash_s[SB_GENERIC_HASH_LEN];
            sstream->read(box_hash_s, SB_GENERIC_HASH_LEN);
            unsigned char box_hash[SB_GENERIC_HASH_LEN];
            std::memcpy(box_hash, box_hash_s, SB_GENERIC_HASH_LEN);

            Hash* hash = new Hash(box_hash);
            Box* box = boxes[hash];
            File* new_file = new File(box->getBaseDir(), hash);
            *sstream >> *new_file;
            if (!file_metadata_written_) {
              new_file->storeMetadata();
              new_file->resize();
              file_metadata_written_ = true;
            }
            delete new_file;

            char* timing_offset_c = new char[8];
            sstream->read(timing_offset_c, 8);

            std::stringstream* message = new std::stringstream();
            *message << SB_SIGTYPE_FSM  << " "
                     << fsm::status_130 << " ";
            message->write(timing_offset_c, 8);
            zmqpp::message z_msg;
            z_msg << message->str();
            z_bo_disp->send(z_msg);
            notified_dispatch_ = true;
          }

          break;
        }
        // STATUS_160
        // acknowledging new file metadata
        case fsm::status_160: {
          file_metadata_written_ = false;
          break;
        }
        // STATUS_170
        // receiving file metadata
        case fsm::status_170: {
          if (state_ == fsm::receiving_file_metadata_change_state) {
            sstream->seekg(1, std::ios_base::cur);
            char box_hash_s[SB_GENERIC_HASH_LEN];
            sstream->read(box_hash_s, SB_GENERIC_HASH_LEN);
            unsigned char box_hash[SB_GENERIC_HASH_LEN];
            std::memcpy(box_hash, box_hash_s, SB_GENERIC_HASH_LEN);

            Hash* hash = new Hash(box_hash);
            Box* box = boxes[hash];
            File* new_file = new File(box->getBaseDir(), hash);
            *sstream >> *new_file;

            if (!new_file->isToBeDeleted() && !file_metadata_written_) {
              if (new_file->exists()) {
                new_file->resize();
              } else {
                new_file->create();
              }
              new_file->storeMetadata();
              file_metadata_written_ = true;
            }
            delete new_file;

            status = fsm::status_173;
            file_metadata_written_ = false;
            event = fsm::get_event_by_status_code(status);
            if ( !check_event(state_, event, status) ) return 1;
          }

          break;
        }
        // STATUS_174
        // receiving file metadata with additional files to come
        case fsm::status_174: {
          if (state_ == fsm::receiving_file_metadata_change_with_more_state) {
            sstream->seekg(1, std::ios_base::cur);
            char box_hash_s[SB_GENERIC_HASH_LEN];
            sstream->read(box_hash_s, SB_GENERIC_HASH_LEN);
            unsigned char box_hash[SB_GENERIC_HASH_LEN];
            std::memcpy(box_hash, box_hash_s, SB_GENERIC_HASH_LEN);

            Hash* hash = new Hash(box_hash);
            Box* box = boxes[hash];
            File* new_file = new File(box->getBaseDir(), hash);
            *sstream >> *new_file;

            if (!new_file->isToBeDeleted() && !file_metadata_written_) {
              if (new_file->exists()) {
                new_file->resize();
              } else {
                new_file->create();
              }
              new_file->storeMetadata();
              // \TODO in status_174 the sender sends the metadata
              //       of all files without changing the status, this 
              //       isn't handled yet
              file_metadata_written_ = true;
            }
            std::memcpy(current_box_, box_hash, SB_GENERIC_HASH_LEN);
            std::stringstream cf;
            cf << *new_file;
            current_file_ << cf.str().substr(32);
            delete new_file;
            notified_dispatch_ = false;

            status = fsm::status_173;
            event = fsm::get_event_by_status_code(status);
            if ( !check_event(state_, event, status) ) return 1;
          }

          break;
        }

        // STATUS_150
        // when receiving 150 in ready_state_, return to normal heartbeat
        case fsm::status_150: {
          if ( state_ == fsm::ready_state ) {
            status = fsm::status_100;
            event = fsm::get_event_by_status_code(status);
            if ( !check_event(state_, event, status) ) return 1;
            fsm::action_t action = fsm::get_action(state_, event, status);
            performAction(event, action, status, state_);
          }

          break;
        }

        default:
          break;
        }
    }


    // ALL_NODES_REPLIED
    // ALL_NODES_HAVE_ALL_METADATA_CHANGES_WITH_MORE && STATUS177
    // The next state requires the dispatcher to fire after
    // a calculated offset
    if ( (  event == fsm::all_nodes_replied_event
        && status == fsm::status_122 )
      || (  event == fsm::all_nodes_have_all_metadata_changes_with_more_event
        && status == fsm::status_177 ) ) {
      // calculating offset, store it and send it to dispatch
      std::stringstream message;
      message << SB_SIGTYPE_FSM << " "
              << status         << " ";

      // jump back to the beginning of the stream to
      // re-read the node_hash
      sstream->seekg(5, std::ios_base::beg);
      char node_hash_s[SB_GENERIC_HASH_LEN];
      sstream->read(node_hash_s, SB_GENERIC_HASH_LEN);
      unsigned char node_hash[SB_GENERIC_HASH_LEN];
      std::memcpy(node_hash, node_hash_s, SB_GENERIC_HASH_LEN);
      Hash* hash = new Hash(node_hash);

      uint64_t timestamp =
        std::chrono::duration_cast< std::chrono::milliseconds >(
          std::chrono::system_clock::now().time_since_epoch()
        ).count();
      uint32_t random_offset = randombytes_uniform(SB_MAXIMUM_OFFSET-SB_MINIMUM_OFFSET);
      random_offset += SB_MINIMUM_OFFSET;
      current_timing_offset_ = timestamp
                               + random_offset
                               - subscribers[hash].offset; // \TODO this should be the average across all nodes
      uint64_t timing_offset = htobe64(current_timing_offset_);
      char* timing_offset_c = new char[8];
      std::memcpy(timing_offset_c, &timing_offset, 8);
      message.write(timing_offset_c, 8);

      char* box_hash = new char[SB_GENERIC_HASH_LEN];
      std::memcpy(box_hash, current_box_, SB_GENERIC_HASH_LEN);
      message.write(box_hash, SB_GENERIC_HASH_LEN);
      Hash* box_hash_obj = new Hash(current_box_);
      Box* box = boxes[box_hash_obj];
      message << box->getBaseDir() << " ";
      message << current_file_.str();

      zmqpp::message z_msg;
      z_msg << message.str();
      z_bo_disp->send(z_msg);
    }

    // NEW_LOCAL_FILE_EVENT || LOCAL_FILE_METADATA_CHANGE_EVENT
    // NEW_LOCAL_FILE_EVENT_WITH_MORE || LOCAL_FILE_METADATA_CHANGE_EVENT_WITH_MORE
    // if the event was new_local_file_event, add the path of the file to the queue;
    // if the state was announcing_new_file_state, we need to check if the reported 
    // file is the same as the last one, otherwise we would push the same file
    // twice; TODO if a file was created and immediately deleted, that file should 
    // be removed and - if necessary - the state should be reverted
    if ( event == fsm::new_local_file_event
      || event == fsm::new_local_file_with_more_event
      || event == fsm::local_file_metadata_change_event
      || event == fsm::local_file_metadata_change_with_more_event ) {
      int inotify_mask;
      *sstream >> inotify_mask;

      char box_hash_s[SB_GENERIC_HASH_LEN];
      sstream->read(box_hash_s, SB_GENERIC_HASH_LEN);
      unsigned char box_hash[SB_GENERIC_HASH_LEN];
      std::memcpy(box_hash, box_hash_s, SB_GENERIC_HASH_LEN);
      std::memcpy(current_box_, box_hash, SB_GENERIC_HASH_LEN);

      std::string path;
      *sstream >> path;

      if ( path.length() > 128 ) {
        std::cerr << "[E]: filepath is too long, syncbox only supports "
                  << "files up to 128 characters (including subdirectories, "
                  << "excluding the base path)" << std::endl;
        return 1;
      }

      std::deque< File* >* file_list;
      if ( event == fsm::new_local_file_event
        || event == fsm::new_local_file_with_more_event ) {
        file_list = &file_list_data_;
      } else {
        file_list = &file_list_metadata_;
      } 

      Hash* hash = new Hash(box_hash);
      Box* box = boxes[hash];
      File* new_file;
      if ((inotify_mask & IN_DELETE) == IN_DELETE) {
        new_file = new File(box->getBaseDir(), hash, path, false, true);
      } else {
        new_file = new File(box->getBaseDir(), hash, path);
      }
      if ( state_ == fsm::announcing_new_file_state ) {
        std::deque<File*>::iterator iter;
        iter = std::find(file_list->begin(), file_list->end(), new_file);
        if ( iter != file_list->end() ) {
          file_list->push_back(new_file);
        }
      } else {
        file_list->push_back(new_file);
      }
      node_reply_counter_ = 0;
    }


    // RECEIVED FILE DATA
    // read the file data and store it
    if ( event == fsm::received_file_data_event ) {
      if (SB_MSG_DEBUG) printf("bo: receiving file data...\n");
      Hash* box_hash = new Hash(current_box_);
      Box* box = boxes[box_hash];
      File* file = new File(box->getBaseDir(), box_hash);
      current_file_ >> *file;

      sstream->seekg(SB_GENERIC_HASH_LEN, std::ios_base::cur);
      char* offset_c = new char[8];
      sstream->read(offset_c, 8);
      uint64_t offset_be;
      std::memcpy(&offset_be, offset_c, 8);
      uint64_t offset = be64toh(offset_be);

      char* data_size_c = new char[8];
      sstream->read(data_size_c, 8);
      uint64_t data_size_be;
      std::memcpy(&data_size_be, data_size_c, 8);
      uint64_t data_size = be64toh(data_size_be);

      char* contents = new char[data_size];
      sstream->read(contents, data_size);
      file->storeFileData(contents, data_size, offset);
      delete contents;
      delete file;
    }


    // FSM CONTINUE
    fsm::state_t new_state = fsm::get_new_state(state_, event, status);
    if ( state_ != new_state ) {
      fsm::action_t action = fsm::get_action(state_, event, status);
      performAction(event, action, status, new_state);
      if (SB_MSG_DEBUG) printf("bo: updating self to state %d\n", 
        fsm::get_new_state(state_, event, status));
      state_ = new_state;
    }

  } else {
    if (SB_MSG_DEBUG) printf("bo: unhandled event, ignoring...\n");
  }

  return 0;
}

int Boxoffice::performAction(fsm::event_t const event, 
                             fsm::action_t const action, 
                             fsm::status_t const received_status,
                             fsm::state_t const new_state) {

  switch (action) {
    case fsm::send_heartbeat_action: {
      fsm::status_t new_status = fsm::get_heartbeat_status(state_, event, received_status);
      updateHeartbeat(new_status, new_state);
      break;
    }
    default:
      if (SB_MSG_DEBUG) printf("bo: could not perform action: %d\n", action);
      break;
  }

  return 0;
}

int Boxoffice::updateHeartbeat(fsm::status_t const new_status,
                               fsm::state_t const new_state) {
  if (SB_MSG_DEBUG) printf("bo: changing status code to %d\n", new_status);
  std::stringstream* message = new std::stringstream();
  *message << SB_SIGTYPE_FSM << " " << new_status << " ";
  prepareHeartbeatMessage(message, new_state);
  zmqpp::message z_msg;
  z_msg << message->str();
  z_bo_hb->send(z_msg);
  delete message;

  return 0;
}

void Boxoffice::prepareHeartbeatMessage(std::stringstream* message, 
                                        fsm::state_t const new_state) {
  *message << "";

  if (        new_state == fsm::sending_new_file_metadata_state
           || new_state == fsm::sending_new_file_metadata_with_more_state ) {
    File* current_file = file_list_data_.front();
    std::stringstream cf;
    cf << *current_file;
    current_file_ << cf.str().substr(32);
    *message << *current_file;
    file_list_data_.pop_front();
    uint64_t timing_offset = htobe64(current_timing_offset_);
    char* timing_offset_c = new char[8];
    std::memcpy(timing_offset_c, &timing_offset, 8);
    message->write(timing_offset_c, 8);
  } else if ( new_state == fsm::sending_file_metadata_change_state
           || new_state == fsm::sending_file_metadata_change_with_more_state) {
    File* current_file = file_list_metadata_.front();
    std::stringstream cf;
    cf << *current_file;
    current_file_ << cf.str().substr(32);
    *message << *current_file;
    file_list_metadata_.pop_front();
  }
}

bool Boxoffice::checkEvent(fsm::state_t const state,
                           fsm::event_t const event,
                           fsm::status_t const status) const {
  if ( !fsm::check_event(state, event, status) ) {
    if (SB_MSG_DEBUG) printf("bo: changed to unhandled event, ignoring...\n");
    return false;
  }
  return true;
}

int Boxoffice::updateTimestamp(std::stringstream* sstream) {
  char node_hash_s[SB_GENERIC_HASH_LEN];
  sstream->read(node_hash_s, SB_GENERIC_HASH_LEN);
  unsigned char node_hash[SB_GENERIC_HASH_LEN];
  std::memcpy(node_hash, node_hash_s, SB_GENERIC_HASH_LEN);
  Hash* hash = new Hash(node_hash);
  uint64_t node_timestamp, local_timestamp;
  uint16_t offset;
  char* node_timestamp_c = new char[8];
  sstream->seekg(1, std::ios_base::cur);
  sstream->read(node_timestamp_c, 8);
  std::memcpy(&node_timestamp, node_timestamp_c, 8);
  subscribers[hash].last_timestamp = be64toh(node_timestamp);
  local_timestamp = std::chrono::duration_cast< std::chrono::milliseconds >(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
  offset = local_timestamp - subscribers[hash].last_timestamp;
  subscribers[hash].offset = offset;

  return 0;
}



void *publisher_thread(zmqpp::context* z_ctx, host_t host)
{
  Publisher* pub = new Publisher(z_ctx, host);

  pub->run();
  pub->sendExitSignal();

  delete pub;

  return (NULL);
}

void *heartbeater_thread(zmqpp::context* z_ctx, fsm::status_t status)
{
  Heartbeater* hb = new Heartbeater(z_ctx, status);

  hb->run();
  hb->sendExitSignal();

  delete hb;

  return (NULL);
}

void *dispatcher_thread(zmqpp::context* z_ctx, fsm::status_t status)
{
  Dispatcher* disp = new Dispatcher(z_ctx, status);

  disp->run();
  disp->sendExitSignal();

  delete disp;

  return (NULL);
}

void *subscriber_thread(zmqpp::context* z_ctx, node_t node)
{
  Subscriber* sub = new Subscriber(z_ctx, node);

  sub->run();
  sub->sendExitSignal();

  delete sub;

  return (NULL);
}

void *box_thread(Box* box)
{
  box->run();
  box->sendExitSignal();

  delete box;

  return (NULL);
}