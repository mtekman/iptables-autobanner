Main:
=====
ban_addresses.sh -- generates and parses journalctl logs to ban addresses who repeatedly 
failes to ssh in

-- evilAddressParser, cpp script that prints a sorted frequency table on ports and users 
attempted for a given address


For a secure system, add this to your crontab as root:

# 10 o'clock every morning and 10 every night
0 10,22 * * * cd /home/tetris/REPOS/personal_scripts/os_specific/arch/iptables/ && ./ban_addresses.sh | tee -a /home/tetris/cron_log.txt

