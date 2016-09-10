/*
 * Config
 */

#include "constants.hpp"
#include "config.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <regex>
#include <boost/filesystem.hpp>
#include <wordexp.h>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>


int Config::initialize(int argc, char* argv[])
{
    Config* c = Config::getInstance();

    std::vector< std::string > nodes;
    std::vector< std::string > hostnames;
    std::vector< std::string > box_strings;
    std::string                configfile;
    std::string                keystore_file;
    std::string                private_key_file;

    // parsing program options using boost::program_options
    namespace po = boost::program_options;

    po::options_description cmdline_options("Generic options");
    cmdline_options.add_options()
        ("help,h", "Produce this help message")
        ("config,c", po::value<std::string>(&configfile),
            "Use supplied config file instead of default one")
        ("keystore", po::value<std::string>(&keystore_file),
            "Use supplied keystore file instead of default one")
        ("privatekey", po::value<std::string>(&private_key_file),
            "Use supplied privatekey file instead of default one")
        ("create-private-key", "Create new keypair for hostnames")
    ;

    po::options_description generic_options("Allowed options");
    generic_options.add_options()
        ("node,n", po::value<std::vector< std::string >>(&nodes), 
            "Add node location to listen to (multiple arguments allowed)")
        ("box,b", po::value<std::vector <std::string> >(&box_strings), 
            "Add path of a directory to watch (multiple arguments allowed)")
        ("hostname,p", po::value<std::vector <std::string> >(&hostnames),
            "Add a name for this machine under which other nodes can reach it (multiple arguments allowed)")
    ;

    po::options_description options;
    options.add(cmdline_options).add(generic_options);


    // BUG we cannot ADD nodes/boxes through the cli,
    //     it takes either the config file or the command line...
    // parse command line arguments
    po::store(po::parse_command_line( argc, argv, options ), c->vm_);
    po::notify(c->vm_);

    // parse config file, if any
    wordexp_t expanded_config_file_path;
    if (configfile.empty()) {
        wordexp( F_CONFIG_FILE, &expanded_config_file_path, 0 );
    } else {
        wordexp( configfile.c_str(), &expanded_config_file_path, 0 );
    }
    std::ifstream ifs( expanded_config_file_path.we_wordv[0] );
    wordfree(&expanded_config_file_path);
    if ( !ifs ) {
        if (F_MSG_DEBUG) printf("config: no config file found\n");
    } else {
        po::store(po::parse_config_file(ifs, generic_options), c->vm_);
        po::notify(c->vm_);
        ifs.close();
    }

    // parse keystore file
    wordexp_t expanded_keystore_file_path;
    if (keystore_file.empty()) {
        wordexp( F_KEYSTORE_FILE, &expanded_keystore_file_path, 0 );
    } else {
        wordexp( keystore_file.c_str(), &expanded_keystore_file_path, 0 );
    }
    keystore_file = expanded_keystore_file_path.we_wordv[0];
    wordfree(&expanded_keystore_file_path);

    // parse privatekey file
    wordexp_t expanded_privatekey_file_path;
    if (private_key_file.empty()) {
        wordexp( F_PRIVATEKEY_FILE, &expanded_privatekey_file_path, 0 );
    } else {
        wordexp( private_key_file.c_str(), &expanded_privatekey_file_path, 0 );
    }
    private_key_file = expanded_privatekey_file_path.we_wordv[0];
    wordfree(&expanded_privatekey_file_path);

    int return_val;
    return_val = c->doSanityCheck( &options, &nodes, &hostnames, &box_strings );
    if ( return_val != 0 ) return return_val;
    return_val = c->synchronizeKeystore( &keystore_file, &private_key_file );
    return return_val;
}

const node_map
    Config::getNodes() const {
        return nodes_;
}
const std::vector< std::string >
  Config::getNodeEndpoints() const {
    std::vector<std::string> endpoints;
    for ( node_map::const_iterator i=nodes_.begin();
          i != nodes_.end(); ++i ) {
        endpoints.push_back( i->second.endpoint );
    }
    return endpoints;
}
const std::vector< std::string >
  Config::getNodePublicKeys() const {
    std::vector<std::string> endpoints;
    for ( node_map::const_iterator i=nodes_.begin();
          i != nodes_.end(); ++i ) {
        endpoints.push_back( i->second.public_key );
    }
    return endpoints;
}
const std::vector< host_t >
    Config::getHosts() const {
        return hosts_;
}
// TODO this is a hack, remove
const zmqpp::curve::keypair
  Config::getHostKeypair() const {
    return hosts_[0].keypair;
}
const std::vector< box_t >
    Config::getBoxes() const {
        return boxes_;
}

int Config::doSanityCheck(boost::program_options::options_description* options, 
                          std::vector<std::string>* nodes, 
                          std::vector<std::string>* hostnames, 
                          std::vector<std::string>* box_strings) {

    // check for help argument, print the help, and exit
    if (vm_.count("help")) {
        std::cout << *(options) << std::endl;
        return 1;
    }

    // checking nodes (need at least one)
    if (F_MSG_DEBUG) printf("config: checking nodes\n");
    if ( nodes->size() >= 1 ) {
        // TODO add logic for non-bidirectional nodes

        // Let's make the nodes vector unique before turning it over
        std::vector<std::string>::iterator i;
        i = std::unique( nodes->begin(), nodes->end() );
        nodes->resize( std::distance( nodes->begin(), i ) );

        for (i = nodes->begin(); i != nodes->end(); ++i)
        {
            // check if the supplied node is connectible
            // only tcp is allowed
            // depending on the matched regex, the zmq protocol will be appended
            std::smatch sm;
            if ( std::regex_match( *i, 
                                   sm, 
                                   std::regex("(tcp://)?(.*)(:)([0-9]*)")
                                 ) )
            {
                // prepare the node endpoint
                std::string endpoint = *i;
                if ( sm[1].length() == 0 )
                    endpoint = "tcp://" + endpoint;

                // add it to the config class for later use
                node_t new_node;
                new_node.endpoint = endpoint;
                new_node.f_subtype = F_SUBTYPE_TCP_BIDIR;
                new_node.last_timestamp = 0;
                new_node.offset = 0;
                this->nodes_vec_.push_back( new_node );
            } else if ( F_MSG_DEBUG && std::regex_match( *i, 
                                   sm, 
                                   std::regex("(ipc://)(.*)")
                                 ) ) {
                // add it to the config class for later use
                node_t new_node;
                new_node.endpoint = *i;
                new_node.f_subtype = F_SUBTYPE_TCP_BIDIR;
                new_node.last_timestamp = 0;
                new_node.offset = 0;
                this->nodes_vec_.push_back( new_node );
            } else {
                std::cerr << "[E] Cannot process node '" << *i << "'" << std::endl;
                return 1;
            }
        }
    } else {
        perror("[E] No nodes supplied, exiting...");
        return 1;
    }

    // checking publishers
    if (F_MSG_DEBUG) printf("config: checking publishers\n");
    if ( hostnames->size() >= 1 ) {
        // Let's make the nodes vector unique before turning it over
        std::vector<std::string>::iterator i;
        i = std::unique( hostnames->begin(), hostnames->end() );
        hostnames->resize( std::distance( hostnames->begin(), i ) );

        for ( i = hostnames->begin(); i != hostnames->end(); ++i)
        {
            // check if the supplied publisher is connectible
            // only tcp is allowed
            // depending on the matched regex, the zmq protocol will be appended
            std::smatch sm;
            if ( std::regex_match( *i, 
                                   sm, 
                                   std::regex("(tcp://)?(.*)(:)([0-9]*)")
                                 ) )
            {
                // prepare the node endpoint
                std::string endpoint = *i;
                if ( sm[1].length() == 0 )
                    endpoint = "tcp://" + endpoint;

                host_t host;
                host.endpoint = endpoint;
                this->hosts_.push_back(host);
            } else if ( F_MSG_DEBUG && std::regex_match( *i, 
                                   sm, 
                                   std::regex("(ipc://)(.*)")
                                 ) ) {
                host_t host;
                host.endpoint = *i;
                this->hosts_.push_back(host);
            } else {
                std::cerr << "[E] Cannot process node '" << *i << "'" << std::endl;
                return 1;
            }
        }
    } else {
        perror("[E] No publishers supplied, exiting...");
        return 1;
    }

    // checking boxes
    if (F_MSG_DEBUG) printf("config: checking boxes\n");
    if ( box_strings->size() >= 1 ) {
        std::vector< std::string >::iterator i;
        for (i = box_strings->begin(); 
             i != box_strings->end(); ++i)
        {
            std::istringstream box_string(*i);
            std::stringstream box_path_sstream;
            std::string box_name_string, box_path, box_path_temp;
            std::getline(box_string, box_name_string, '@');
            Hash box_name(box_name_string);
            std::getline(box_string, box_path_temp, '@');
            box_path_sstream << box_path_temp;
            while(std::getline(box_string, box_path_temp, '@')) {
                box_path_sstream << "@" << box_path_temp;
            }
            box_path = box_path_sstream.str();

            // check if the box locations map to directories on the filesystem
            if ( !boost::filesystem::is_directory(box_path) ) {
                std::cerr << "[E] " << box_path << " is not a directory." << std::endl;
                return 1;
            }
            // check if the box location is already mapped or the box name has already been claimed
            for (std::vector<box_t>::iterator j = this->boxes_.begin();
                 j != this->boxes_.end(); ++j) {
                if (std::memcmp(box_name.getBytes(), j->uid, F_GENERIC_HASH_LEN) == 0 ) {
                    std::cerr << "[E] " << box_name.getBytes() << " is already used." << std::endl;
                    return 1;
                }
                if (box_path == j->base_path) {
                    std::cerr << "[E] " << box_path << " is already mapped." << std::endl;
                    return 1;
                }
            }

            // add new box strings to Config
            box_t new_box;
            std::memcpy(new_box.uid, box_name.getBytes(), F_GENERIC_HASH_LEN);
            new_box.base_path = box_path;
            this->boxes_.push_back( new_box );
        }
    } else {
        perror("[E] No box locations supplied, exiting...");
        return 1;
    }

    return 0;
}

int Config::synchronizeKeystore( std::string* keystore_file, 
                                 std::string* private_key_file ) {

    // Getting all private keys for hostnames
    // Generating new keypairs for hostnames on flag create-private-key
    Json::Value pks;
    std::fstream f_pk( *private_key_file );
    if ( !f_pk && !vm_.count("create-private-key") ) {
        std::cerr << "[E] Could not open the private key file. Please check "
                  << "your permissions" << std::endl;
        return 1;
    } else {
        Json::Reader json_reader;
        if ( !json_reader.parse(f_pk, pks) ) {
            std::cerr << "[E] error parsing " << *private_key_file << std::endl;
            return 1;
        }
        f_pk.close();
    }

    // Writing keypairs for hostnames on flag create-private-key
    if ( vm_.count("create-private-key") ) {
        f_pk.open( *private_key_file, std::fstream::out );
        std::cout << "config: Generating keys for hosts" << std::endl;
        for (std::vector<host_t>::iterator i = hosts_.begin(); 
             i != hosts_.end(); ++i) {
            if ( !pks.isMember(i->endpoint) ) {
                i->keypair = zmqpp::curve::generate_keypair();
                i->uid = Hash(i->keypair.public_key).getString();
                pks[i->endpoint]["public_key"] = i->keypair.public_key;
                pks[i->endpoint]["private_key"] = i->keypair.secret_key;
            } else {
                i->keypair.public_key = pks[i->endpoint].get("public_key", "").asString();
                i->keypair.secret_key = pks[i->endpoint].get("private_key", "").asString();
                i->uid = Hash(i->keypair.public_key).getString();
            }
        }
        f_pk << pks << std::endl;
        std::cout << "config: Successfully written keys to "
                  << *private_key_file << std::endl;
        f_pk.close();
        return 1;
    } else {
        for (std::vector<host_t>::iterator i = hosts_.begin(); 
             i != hosts_.end(); ++i) {
            i->keypair.public_key = pks[i->endpoint].get("public_key", "").asString();
            i->keypair.secret_key = pks[i->endpoint].get("private_key", "").asString();
            i->uid = Hash(i->keypair.public_key).getString();
        }
        f_pk.close();
    }

    // Reading the keystore
    std::ifstream if_ks( *keystore_file );
    Json::Value ks;
    if ( !if_ks ) {
        std::cerr << "[E] Could not find the keystore. Please share all "
                  << "public keys before starting flocksy" << std::endl;
        return 1;
    } else {
        Json::Reader json_reader;
        if ( !json_reader.parse(if_ks, ks) ) {
            std::cerr << "[E] error parsing " << *keystore_file << std::endl;
            return 1;
        }
        if_ks.close();
    }

    for ( std::vector<node_t>::iterator i = this->nodes_vec_.begin();
          i != this->nodes_vec_.end(); ++i ) {
        i->public_key = ks.get(i->endpoint, "").asString();
        Hash* hash = new Hash(i->public_key);
        std::memcpy(i->uid, hash->getBytes(), F_GENERIC_HASH_LEN);
        nodes_.insert( std::make_pair(hash, *i) );
    }

    return 0;
}
