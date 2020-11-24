/*
 huxley_jail
 Executar um comando num ambiente protegido
 2014
 */
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>

#include <string.h>
#include <errno.h>

/* Número de nano segundos que correspondem a um segundo */
#define NANO_SECOND 1000000000
/* tempo de erro entre o sinal e o estouro no caso de time-limit */
#define EPSILON 0.01
#define MBYTE  (1024*1024)
#define BUFFSIZE 256
#define DEBUG 0

const char vers[] = "1.0";
int child_pid; /* pid of the child process */
struct rlimit cpu_timeout = { 5, 5 }; /* max cpu time (seconds) */
char rootdir[BUFFSIZE], saida[BUFFSIZE], entrada[BUFFSIZE], erro[BUFFSIZE];

enum EXIT_CODES {
	NORMAL = 0,
	INCORRECT_INVOCATION,
	RUNTIME_ERROR,
	TIME_OUT,
	CPU_LIMIT_ERROR,
	UNABLE_TO_EXECUTE_ERROR
};

/* alarm handler */
void handle_alarm(int sig) {
	if (DEBUG) {
		fprintf(stderr, "timed-out (cputime) after %d seconds\n",
				(int) cpu_timeout.rlim_max);
		fflush(stderr);
	}
	kill(child_pid,9);

	exit(TIME_OUT);
}

void usage(int argc, char **argv) {
	fprintf(stderr,
			"huxley_jail version %s\nusage: %s [ options ] cmd [ arg1 arg2 ... ]\n",
			vers, argv[0]);
	fprintf(stderr, "available options are:\n");

	fprintf(stderr, "\t-t <max cpu time> (default: %d secs)\n",
			(int) cpu_timeout.rlim_max);

	fprintf(stderr, "\t-i <standard input file> (default: not defined)\n");
	fprintf(stderr, "\t-o <standard output file> (default: not defined)\n");
	fprintf(stderr, "\t-e <standard error file> (default: not defined)\n");

	/*******Note that currently Linux does not support memory usage limits********/
}

int timespec_subtract(result, x, y)
	struct timespec *result, *x, *y; {
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_nsec < y->tv_nsec) {
		int nsec = (y->tv_nsec - x->tv_nsec) / NANO_SECOND + 1;
		y->tv_nsec -= NANO_SECOND * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_nsec - y->tv_nsec > NANO_SECOND) {
		int nsec = (x->tv_nsec - y->tv_nsec) / NANO_SECOND;
		y->tv_nsec += NANO_SECOND * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_nsec = x->tv_nsec - y->tv_nsec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}



void check_params(int argc, char **argv) {
	int opt;
	if (argc > 1 && !strcmp("--help", argv[1])) {
		usage(argc, argv);
		exit(INCORRECT_INVOCATION);
	}

	/* parse command-line options */
	getcwd(rootdir, BUFFSIZE); /* default: use cwd as rootdir */
	while ((opt = getopt(argc, argv, "t:i:o:e:?")) != -1) {
		switch (opt) {

		case 't':
			cpu_timeout.rlim_max = cpu_timeout.rlim_cur = atoi(optarg);
			break;
		case 'i':
			strncpy(entrada, optarg, 255);
			break;
		case 'o':
			strncpy(saida, optarg, 255);
			break;
		case 'e':
			strncpy(erro, optarg, 255);
			break;

		case '?':
			usage(argc, argv);
			exit(INCORRECT_INVOCATION);
		}
	}

	if (optind >= argc) { /* no more arguments */
		usage(argc, argv);
		exit(INCORRECT_INVOCATION);
	}
}

/*
    Depois que foi feito o upgrade do servidor do linode, a forma antiga de
    medir o tempo de execućão através do getrusage parou de funcionar.
    Ele passou a retornar sempre 0.
    Sendo assim, passamos a utilizar o "relógio de parede".
*/
double get_diff(struct timespec *t2, struct timespec *t1)
{
    struct timespec result;
    timespec_subtract(&result,t2,t1);
    return (result.tv_sec * NANO_SECOND + result.tv_nsec)/(double)NANO_SECOND;
}

int main(int argc, char **argv) {
	int status;
	int exit_code;
	int is_python = 0, is_octave = 0, is_java = 0;

	double dt;
	struct timespec tv1, tv2, result;

	entrada[0] = saida[0] = erro[0] = 0;

	check_params(argc, argv);



	if ((child_pid = fork())) {
		/* ------------------- parent process ----------------------------- */
		clock_gettime(CLOCK_MONOTONIC,&tv1);

		if (strstr(argv[optind], "python") != NULL) {
			is_python = 1;
		} else if (strstr(argv[optind], "run_octave") != NULL) {
			is_octave = 1;
		} else if (strstr(argv[optind], "java") != NULL) {
            is_java = 1;
        }

		if (cpu_timeout.rlim_max > 0) {
			alarm(cpu_timeout.rlim_max); // set alarm and wait for child execution
			signal(SIGALRM, handle_alarm);
		}

		wait(&status);
		clock_gettime(CLOCK_MONOTONIC,&tv2);

		dt = get_diff(&tv2,&tv1);

		// essa aqui é só uma verficacao extra, caso o alarme
		// acima não funcione.
		if (dt + EPSILON >= cpu_timeout.rlim_max) {
			if (DEBUG) {
				fprintf(stderr, "timed-out (cputime) after %d seconds\n",
						(int) cpu_timeout.rlim_max);
				fflush(stderr);
			}
			exit_code = TIME_OUT;

		} else if (WIFSIGNALED(status)) {
			// check if child got an uncaught signal error & reproduce it in parent
			if (DEBUG) {
				fprintf(stderr,
						"RUN-TIME SIGNAL REPORTED BY THE PROGRAM %s: %d\n",
						argv[optind], WTERMSIG(status));
				fflush(stderr);
			}

			if (WTERMSIG(status) == 9)
			{
			    //Algum processo externo matou esse processo
			    exit_code = TIME_OUT;
			}
			else
			{
			    exit_code = RUNTIME_ERROR;
			}

		} else if (WIFEXITED(status)) {
			exit_code = WEXITSTATUS(status);
//			fprintf(stderr, "is_java= %d, exit_code: %d\n", is_java, exit_code);
//			fprintf(stderr,"EXITSTATUS %d, isoctave:%d \n",exit_code,is_octave);
//			fprintf(stderr,"parameter: %s\n",argv[optind]);
			if ((is_python || is_octave || is_java) && exit_code == 1) {
				// In python or octave, when the program has an unhandled exception, it returns 1
				// we need to transform it in a runtime error
				if (DEBUG) {
					fprintf(stderr, "Java, Python or Octave RUN-TIME ERROR\n");
					fflush(stderr);
				}
				exit_code = RUNTIME_ERROR;

			} else if (exit_code) {
				if (DEBUG) {
					fprintf(stderr,
							"NORMAL EXECUTION BUT PROGRAM [%s] EXITED WITH NONZERO CODE: [%d]\n",
							argv[optind], exit_code);
					fflush(stderr);
				}
				exit_code = NORMAL;
			} else {
				if (DEBUG) {
					fprintf(stderr, "NORMAL EXECUTION %s\n", argv[optind]);
					fflush(stderr);
				}
				exit_code = NORMAL;
			}

		} else {
			if (DEBUG) {
				fprintf(stderr, "PROGRAM TERMINATED ABNORMALLY %s\n",
						argv[optind]);
				fflush(stderr);
			}
			exit_code = RUNTIME_ERROR;
		}

		fprintf(stderr, "CPU_TIME_RUNNING:%lfs\n", dt);
		exit(exit_code);
	} else {

		/* ------------------- child process ------------------------------ */
		if (saida[0]) {
			freopen(saida, "w", stdout);
		}
		if (erro[0])
			freopen(erro, "w", stderr);
		if (entrada[0])
			freopen(entrada, "r", stdin);

		/* attempt to change the hard limits */
		/*******Note that currently Linux does not support memory usage limits********/
		if (setrlimit(RLIMIT_CPU, &cpu_timeout)) {
			if (DEBUG) {
				fprintf(stderr, "%s\n", strerror(errno));
				fprintf(stderr, "%s: can't set hard limits\n", argv[0]);
			}
			exit(CPU_LIMIT_ERROR);
		}
		if (execv(argv[optind], &argv[optind]) < 0) {
			if (DEBUG) {
				fprintf(stderr, "%s\n", strerror(errno));
				fprintf(stderr, "%s: unable to exec %s\n", argv[0],
						argv[optind]);
			}
			exit(UNABLE_TO_EXECUTE_ERROR);
		}
	}
	exit(NORMAL);
}
