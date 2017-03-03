	#ifndef _SYSTEM_LOG_H_
#define _SYSTEM_LOG_H_

#include <syslog.h>
#include <stdbool.h>

// openlog(), closelog(), syslog(), vsyslog() are thread safe!

// Follow this Link: http://unix.stackexchange.com/questions/40533/lubuntu-12-04-syslog-to-custom-file-not-var-log-syslog-but-var-log-mylog
// http://man7.org/linux/man-pages/man3/syslog.3.html
// http://www.networkmanagementsoftware.com/what-is-syslog/

#define INIT_LOGLINE_DEFAULT(cstr)	init_logline(cstr, false, true, true)
#define CLOSE_LOGLINE()				closelog()

// If the mask argument is 0, the current logmask is not modified.
// It returns the previous mask
#define MASK_LOGLINE(mask)			setlogmask(mask) // The initial mask is such that logging is enabled for all priorities.


// The __VA_ARGS__ arguments are  a format, as in printf(3) and any arguments required by the format,
// except that the two character sequence %m will be replaced by the error message string strerror(errno).

// This determines the importance of the message.  The levels are, in order of decreasing importance:

#define LOGLINE_EMERGENCY(...) 		syslog(LOG_EMERG, __VA_ARGS__)
#define LOGLINE_ALERT(...)			syslog(LOG_ALERT, __VA_ARGS__)
#define LOGLINE_CRITICAL(...)  		syslog(LOG_CRIT, __VA_ARGS__)
#define LOGLINE_ERROR(...)			syslog(LOG_ERR, __VA_ARGS__)
#define LOGLINE_WARNING(...)		syslog(LOG_WARNING, __VA_ARGS__)
#define LOGLINE_NOTICE(...)			syslog(LOG_NOTICE, __VA_ARGS__)
#define LOGLINE_INFO(...)			syslog(LOG_INFO, __VA_ARGS__)
#define LOGLINE_DEBUG(...)			syslog(LOG_DEBUG, __VA_ARGS__)



// The argument ident in the call of openlog() is probably stored as-is.
// Thus, if the string it points to is changed, syslog() may start
// prepending the changed string, and if the string it points to ceases
// to exist, the results are undefined.  Most portable is to use a
// string constant.

inline void init_logline(const char *cstr, bool print_to_strerr, bool print_pid, bool fallback_to_console)
{
	int options = LOG_NDELAY | LOG_NOWAIT;

	if (print_to_strerr)
		options |= LOG_PERROR;
	if (print_pid)
		options |= LOG_PID;
	if (fallback_to_console)
		options |= LOG_CONS;

	openlog(cstr, options, LOG_LOCAL0); // LOG_LOCAL0 through LOG_LOCAL7 reserved for local use
}

//  The setlogmask() function sets this logmask for the calling process,
// and returns the previous mask.  If the mask argument is 0, the
// current logmask is not modified.

inline int mask_logline_upto(int mask)
{
	int upto_mask = 0x00;
	if (mask == LOG_EMERG)
		upto_mask = LOG_EMERG;
	else if (mask == LOG_ALERT)
		upto_mask = LOG_ALERT | LOG_EMERG;
	else if (mask == LOG_CRIT)
		upto_mask = LOG_CRIT | LOG_ALERT | LOG_EMERG;
	else if (mask == LOG_ERR)
		upto_mask = LOG_ERR | LOG_CRIT | LOG_ALERT | LOG_EMERG;
	else if (mask == LOG_WARNING)
		upto_mask = LOG_WARNING | LOG_ERR | LOG_CRIT | LOG_ALERT | LOG_EMERG;
	else if (mask == LOG_NOTICE)
		upto_mask = LOG_NOTICE | LOG_WARNING | LOG_ERR | LOG_CRIT | LOG_ALERT | LOG_EMERG;
	else if (mask == LOG_INFO)
		upto_mask = LOG_INFO | LOG_NOTICE | LOG_WARNING | 
				LOG_ERR | LOG_CRIT | LOG_ALERT | LOG_EMERG;
	else if (mask == LOG_DEBUG)
		upto_mask = LOG_DEBUG | LOG_INFO | LOG_NOTICE | LOG_WARNING | 
				LOG_ERR | LOG_CRIT | LOG_ALERT | LOG_EMERG;
	else
		upto_mask = 0;

	return setlogmask(upto_mask);
}


#endif