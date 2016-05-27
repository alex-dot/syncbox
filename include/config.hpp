/* 
 * The config singleton provides an interface to load and save config files
 * as well as providing access to single settings
 * These settings are uniform for all boxes and subscribers. 
 */

#ifndef SB_CONFIG_HPP
#define SB_CONFIG_HPP

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

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
    const std::vector< std::pair<std::string,int> >
        getSubscriberEndpoints() const;
    const std::vector< std::string >
        getPublisherEndpoints() const;
    const std::vector< std::string >
        getBoxDirectories() const;

  private:
    Config() {};
    ~Config() {};

    int doSanityCheck(boost::program_options::options_description* options, 
                      std::vector<std::string>*);

    boost::program_options::variables_map   vm_;

    std::vector< std::pair<std::string,int> >  subscriber_endpoints_;
    std::vector< std::string >                 publisher_endpoints_;
    std::vector< std::string >                 box_dirs_;

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