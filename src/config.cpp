/*
 * Config
 */

#include "constants.hpp"
#include "config.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <regex>


int Config::initialize(int argc, char* argv[])
{
    Config* c = Config::getInstance();

    // we handle nodes differently, so we want to parse them in a temp variable
    std::vector< std::string > nodes;

    // parsing program options using boost::program_options
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("node,n", po::value<std::vector< std::string >>(&nodes), 
            "Add node to listen to (multiple arguments allowed)")
        ("box,b", po::value<std::vector <std::string> >(&c->box_dirs_), 
            "Add path of a directory to watch (multiple arguments allowed)")
    ;

    po::store(po::parse_command_line( argc, argv, desc ), c->vm_);
    po::notify(c->vm_);

    return c->doSanityCheck( &desc, &nodes );
}

const std::vector< std::pair<std::string,int> >
    Config::getSubscriberEndpoints() const {
        return subscriber_endpoints_;
}

const std::vector< std::string >
    Config::getBoxDirectories() const {
        return box_dirs_;
}

int Config::doSanityCheck(boost::program_options::options_description* desc, 
                          std::vector<std::string>* nodes) {

    // check for help argument, print the help, and exit
    if (vm_.count("help")) {
        std::cout << *(desc) << std::endl;
        return 1;
    }

    Config* c = Config::getInstance();

    // checking nodes (need at least one)
    if (SB_MSG_DEBUG) printf("config: checking nodes\n");
    if ( nodes->size() >= 1 ) {
        // TODO add logic for non-bidirectional nodes
        for (std::vector<std::string>::iterator i = nodes->begin(); 
             i != nodes->end(); ++i)
        {
            // check if the supplied node is connectable
            // only tcp and ipc are allowed
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
                c->subscriber_endpoints_.push_back(
                    std::make_pair( endpoint, SB_SUBTYPE_TCP_BIDIR )
                );
            }
            else if ( std::regex_match( *i, 
                                        sm, 
                                        std::regex("(ipc://)?(.*)(\\.ipc)")
                                      ) )
            {
                // prepare the node endpoint
                std::string endpoint = *i;
                if ( sm[1].length() == 0 )
                    endpoint = "ipc://" + endpoint;

                // add it to the config class for later use
                c->subscriber_endpoints_.push_back(
                    std::make_pair( endpoint, SB_SUBTYPE_TCP_BIDIR )
                );
            } else {
                std::cerr << "[E] Cannot process node '" << *i << "'" << std::endl;
                return 1;
            }
        }
    } else {
        perror("[E] No nodes supplied, exiting...");
        return 1;
    }

    // checking boxes
    if (SB_MSG_DEBUG) printf("config: checking boxes\n");
    if ( c->box_dirs_.size() >= 1 ) {
        for (std::vector< std::string >::iterator i = c->box_dirs_.begin(); 
             i != c->box_dirs_.end(); ++i)
        {
            // check if the box locations map to directories on the filesystem
            if ( !boost::filesystem::is_directory(*i) ) {
                std::cerr << "[E] " << *i << " is not a directory." << std::endl;
                return 1;
            }
        }
    } else {
        perror("[E] No box locations supplied, exiting...");
        return 1;
    }

    return 0;
}
