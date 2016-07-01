/* 
 * The config singleton provides an interface to load and save config files
 * as well as providing access to single settings
 * These settings are uniform for all boxes and subscribers. 
 */

#ifndef SB_CONFIG_HPP
#define SB_CONFIG_HPP

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <hash.hpp>

struct node_t {
  std::string                endpoint;
  std::pair<std::string,int> subscriber;
  uint32_t                   last_timestamp;
  uint16_t                   offset;
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
    const std::vector< std::string >
        getPublisherEndpoints() const;
    const std::vector< box_t >
        getBoxDirectories() const;

  private:
    Config() {};
    ~Config() {};

    int doSanityCheck(boost::program_options::options_description* options, 
                      std::vector<std::string>* nodes,
                      std::vector<std::string>* hostnames,
                      std::vector<std::string>* box_strings);

    boost::program_options::variables_map   vm_;

    std::vector< node_t >       subscribers_;
    std::vector< std::string >  publisher_endpoints_;
    std::vector< box_t >        box_dirs_;

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