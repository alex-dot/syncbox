/* 
 * The config singleton provides an interface to load and save config files
 * as well as providing access to single settings
 * These settings are uniform for all hosts, nodes and boxes. 
 */

#ifndef SB_CONFIG_HPP
#define SB_CONFIG_HPP

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <zmqpp/curve.hpp>

#include <hash.hpp>

// TODO What if I have multiple publishers? Nodes must have a way to query the correct host keypair...
struct node_t {
  std::string  endpoint;
  int          sb_subtype;
  uint32_t     last_timestamp;
  uint16_t     offset;
  std::string  public_key;
  Hash*        uid;
};
struct host_t {
  std::string           endpoint;
  zmqpp::curve::keypair keypair;
  Hash*                 uid;
};
struct box_t {
  Hash*       uid;
  std::string base_path;
};

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
    const std::vector< node_t >
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

    std::vector< node_t >  nodes_;
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