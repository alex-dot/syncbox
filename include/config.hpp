/* 
 * The config singleton provides an interface to load and save config files
 * as well as providing access to single settings
 * These settings are uniform for all boxes and subscribers. 
 */

#ifndef SB_CONFIG_HPP
#define SB_CONFIG_HPP

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <zmqpp/curve.hpp>

#include <hash.hpp>

struct node_t {
  std::string                endpoint;
  std::pair<std::string,int> subscriber;
  uint32_t                   last_timestamp;
  uint16_t                   offset;
  std::string                public_key;
  Hash*                      uid;
};
struct host_t {
  std::string           endpoint;
  zmqpp::curve::keypair keypair;
  Hash*                 uid;
};
typedef std::pair<Hash*,std::string> box_t;

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
        getSubscriberEndpoints() const;
    const std::vector< host_t >
        getPublisherEndpoints() const;
    const std::vector< box_t >
        getBoxDirectories() const;

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

    std::vector< node_t >  subscribers_;
    std::vector< host_t >  hosts_;
    std::vector< box_t >   box_dirs_;

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