# Main:

### ban_addresses.sh 
Generates and parses journalctl logs to ban addresses who repeatedly failes to ssh into:

### evilAddressParser

A cpp script that prints a sorted frequency table on ports and users attempted for a given address

For a secure system, add this to your crontab as root:

    # 10 o'clock every morning and 10 every night
    0 10,22 * * * cd /path/to/repo && ./ban_addresses.sh | tee -a $HOME/cron_log.txt

