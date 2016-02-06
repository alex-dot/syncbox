/* 
 * The config singleton provides an interface to load and save config files
 * as well as providing access to single settings
 * These settings are uniform for all boxes and subscribers. 
 */

#ifndef SB_CONFIG_HPP
#define SB_CONFIG_HPP

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

    static Config* initialize();

  private:
    Config() {};
    ~Config() {};

    int                                        config_backup_type_;
    boost::filesystem::path                    backup_dir_;
    int                                        backup_max_;
    // for backup_config:
    // excludes
    // includes
    // backup-regex?
    // backupprefix?
};


#endif