#pragma once

enum printk_log_level {
	KERN_EMERG,
	KERN_ALERT,
	KERN_CRIT,
	KERN_ERR,
	KERN_WARNING,
	KERN_NOTICE,
	KERN_INFO,
	KERN_DEBUG,
	KERN_DEFAULT,
};

void printk(enum printk_log_level level, const char *fmt, ...);

#define pr_emerg(fmt, ...) printk(KERN_EMERG, fmt, ##__VA_ARGS__)
#define pr_alert(fmt, ...) printk(KERN_ALERT, fmt, ##__VA_ARGS__)
#define pr_crit(fmt, ...) printk(KERN_CRIT, fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...) printk(KERN_ERR, fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...) printk(KERN_WARNING, fmt, ##__VA_ARGS__)
#define pr_notice(fmt, ...) printk(KERN_NOTICE, fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...) printk(KERN_INFO, fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...) printk(KERN_DEBUG, fmt, ##__VA_ARGS__)
#ifdef DEBUG
#define pr_devel(fmt, ...) printk(KERN_DEBUG, fmt, ##__VA_ARGS__)
#endif