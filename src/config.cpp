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


int Config::initialize(int argc, char* argv[])
{
    Config* c = Config::getInstance();

    // we handle nodes differently, so we want to parse them in a temp variable
    std::vector< std::string > nodes;
    std::vector< std::string > hostnames;

    // parsing program options using boost::program_options
    namespace po = boost::program_options;

    po::options_description cmdline_options("Generic options");
    cmdline_options.add_options()
        ("help,h", "produce help message")
    ;

    po::options_description generic_options("Allowed options");
    generic_options.add_options()
        ("node,n", po::value<std::vector< std::string >>(&nodes), 
            "Add node location to listen to (multiple arguments allowed)")
        ("box,b", po::value<std::vector <std::string> >(&c->box_dirs_), 
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
    wordexp( SB_CONFIG_FILE, &expanded_config_file_path, 0 );
    std::ifstream ifs( expanded_config_file_path.we_wordv[0] );
    if ( !ifs ) {
        if (SB_MSG_DEBUG) printf("config: no config file found\n");
    } else {
        po::store(po::parse_config_file(ifs, generic_options), c->vm_);
        po::notify(c->vm_);
    }

    return c->doSanityCheck( &options, &nodes, &hostnames );
}

const std::vector< std::pair<std::string,int> >
    Config::getSubscriberEndpoints() const {
        return subscriber_endpoints_;
}
const std::vector< std::string >
    Config::getPublisherEndpoints() const {
        return publisher_endpoints_;
}
const std::vector< std::string >
    Config::getBoxDirectories() const {
        return box_dirs_;
}

int Config::doSanityCheck(boost::program_options::options_description* options, 
                          std::vector<std::string>* nodes, 
                          std::vector<std::string>* hostnames) {

    // check for help argument, print the help, and exit
    if (vm_.count("help")) {
        std::cout << *(options) << std::endl;
        return 1;
    }

    Config* c = Config::getInstance();

    // checking nodes (need at least one)
    if (SB_MSG_DEBUG) printf("config: checking nodes\n");
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

    // checking publishers
    if (SB_MSG_DEBUG) printf("config: checking publishers\n");
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

                // add it to the config class for later use
                c->publisher_endpoints_.push_back(endpoint);
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
    if (SB_MSG_DEBUG) printf("config: checking boxes\n");
    if ( c->box_dirs_.size() >= 1 ) {
        std::vector< std::string >::iterator i;
        for (i = c->box_dirs_.begin(); 
             i != c->box_dirs_.end(); ++i)
        {
            // check if the box locations map to directories on the filesystem
            if ( !boost::filesystem::is_directory(*i) ) {
                std::cerr << "[E] " << *i << " is not a directory." << std::endl;
                return 1;
            }
        }
        // finally make the vector unique
        i = std::unique( c->box_dirs_.begin(), c->box_dirs_.end() );
        c->box_dirs_.resize( std::distance( c->box_dirs_.begin(), i ) );
    } else {
        perror("[E] No box locations supplied, exiting...");
        return 1;
    }

    return 0;
}
