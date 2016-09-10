/* 
 * The config singleton provides an interface to load and save config files
 * as well as providing access to single settings
 * These settings are uniform for all hosts, nodes and boxes. 
 */

#ifndef F_CONFIG_HPP
#define F_CONFIG_HPP

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <zmqpp/curve.hpp>

class Config
{
  public:
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config* getInstance()
    {
      static Config conf_instance_;
      return &conf_instance_;
    }

    static int initialize(int argc, char* argv[]);

    // Getters
    const node_map
        getNodes() const;
    const std::vector< std::string >
        getNodeEndpoints() const;
    const std::vector< std::string >
        getNodePublicKeys() const;
    const std::vector< host_t >
        getHosts() const;
    const std::vector< std::string >
        getHostEndpoints() const;
    // TODO this is a hack, remove
    const zmqpp::curve::keypair
        getHostKeypair() const;
    const std::vector< box_t >
        getBoxes() const;

  private:
    Config() {};
    ~Config() {};

    int doSanityCheck(boost::program_options::options_description* options, 
                      std::vector<std::string>* nodes,
                      std::vector<std::string>* hosts_,
                      std::vector<std::string>* box_strings);
    int synchronizeKeystore( std::string* keystore_file, 
                             std::string* private_key_file );

    boost::program_options::variables_map   vm_;

    std::vector< node_t >  nodes_vec_;
    node_map               nodes_;
    std::vector< host_t >  hosts_;
    std::vector< box_t >   boxes_;

//    int                                        config_backup_type_;
//    boost::filesystem::path                    backup_dir_;
//    int                                        backup_max_;
    // for backup_config:
    // excludes
    // includes
    // backup-regex?
    // backupprefix?
};


#endif