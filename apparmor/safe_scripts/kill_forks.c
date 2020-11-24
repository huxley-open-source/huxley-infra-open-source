#include<stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#define LINE_LENGTH 300
#define INITIAL_CAPACITY 10
#define THRESHOLD 50
/*
gcc killer.c -o killer `pkg-config --cflags glib-2.0 --libs glib-2.0`
*/


char * get_time()
{
        time_t timer;
        char * buffer = (char *) malloc(25*sizeof(char));
        struct tm* tm_info;

        time(&timer);
        tm_info = localtime(&timer);

        strftime(buffer, 25, "%d/%m/%Y : %H:%M:%S", tm_info);
        return buffer;

}


char** run_cmd(char *cmd, int *lines)
{
	FILE *command_pipe = NULL;
	char **result;

	char line[LINE_LENGTH];
	
	int size = INITIAL_CAPACITY;
	int i;
	int tries = 0;

	*lines=0;

	result = (char**) malloc (size * sizeof(char *) );
	for (i=0; i< size ; i++)
	{
		result[i] = (char *) malloc (LINE_LENGTH * sizeof(char));
	}	
	while (command_pipe==NULL)
	{
		/* Se estiver ocorrendo um fork_bomb, ele terá dificuldades para
		   abrir mais um arquivo, portanto a gente vai ficar tentando
		   e esperando uma brecha
		*/		
		command_pipe = popen(cmd, "r");
		//tries++;
	}

	if (command_pipe)
	{		
		while( !feof( command_pipe ) )
		{	
			if( fgets( line, LINE_LENGTH, command_pipe ) != NULL )
			{		
				if (*lines == size)
				{
					size*=2;
					result = (char**) realloc (result, size * sizeof(char *) );
					for (i=size/2; i< size ; i++)
					{
						result[i] = (char *) malloc (LINE_LENGTH * sizeof(char));
					}
				}
				strcpy(result[*lines], line);
				(*lines)++;
			}
		}
		fclose(command_pipe);
	}
	else
	{
		fprintf(stderr,"WARNING: Could not execute command %s. Could not open it.\n",cmd);
	}
	
	return result;
}

void test_run_cmd()
{
	int lines,i;
	char **result = run_cmd("ls -l", &lines);
	for (i=0 ; i < lines ; i++)
	{
		puts(result[i]);
	}	
}


char * get_name(int pid)
{
	int lines;
	char cmd[255];	
	//sprintf(cmd, "ps -p %d -o comm=",pid);
	sprintf(cmd, "cat /proc/%d/cmdline",pid);
	char **result = run_cmd(cmd,&lines);
	
	if (lines!=1)
	{
		fprintf(stderr, "WARNING: It was not possible to get a name for pid: [%d]\n",pid);
		return NULL;
	}
	return result[0];

}

void test_get_name()
{
	int my_pid = getpid();
	puts(get_name(my_pid));
}


void kill_tree_by_name(char *p_name)
{
	int lines;
	char cmd[255];
	char ** result;

	if (p_name)
	{	
		// Mata todos com o mesmo nome de processo. Provavelmente vai matar logo os filhos
		sprintf(cmd, "pkill -9 -x %s",p_name);
		result = run_cmd(cmd, &lines);		
		free(result);
		free(p_name);
	}
	else
	{
		fprintf(stderr,"WARNING: could not KILL %s\n",p_name);
		
	}
	
	// De qualquer forma, verifica se ainda sobraram filhos, com o mesmo nome ou com
	// nomes diferentes
/*
	sprintf(cmd, "pgrep -P %d",pid);
	result = run_cmd(cmd, &lines);
	int i, pids[lines];
	for (i=0; i < lines ; i++)
	{
		// mata recursivamente
		kill_tree(atoi(result[i]));
	}
	free(result);
*/
}

void kill_tree(int pid)
{
	printf("kill_tree %d\n",pid);
	char *p_name = get_name(pid);
	kill_tree_by_name(p_name);	

}

void kill_list(gpointer key, gpointer value, gpointer userdata)
{	
	int int_value = atoi(value);
	if (int_value > THRESHOLD)
	{
		printf("%s .Killing: %s\n",get_time(), (char *) key);
		kill_tree_by_name( (char *) key );
	}
}

/**
	Conta quantas instâncias existem de cada processo. Se ultrapassar o THRESHOLD,
	ele chama a função kill_list que mata cada uma delas.
 */
void kill_forks()
{	
	GHashTable *table=g_hash_table_new(g_str_hash, g_str_equal);

	if (table==NULL)
	{
		fprintf(stderr,"CANNOT CREATE HASH TABLE\n");
	}

	int lines;
	int i;	
	char **result = run_cmd("ps -Ao \"%c\"",&lines);
	int counter;
	
	for (i =0 ; i < lines ; i++)
	{		
		gchar *value = (gchar *) g_hash_table_lookup(table,result[i]);
		
		counter = 1;		
		if (value!=NULL)
		{	
			counter = atoi(value) +1;
		}
		else
		{
			value = (gchar *)malloc(10*sizeof(gchar));
		}
		sprintf((char *)value, "%d",counter);
		g_hash_table_insert(table,result[i],value);
	}
	g_hash_table_foreach(table, kill_list, NULL);
}




int main(){
//	test_run_cmd();
//	test_get_name();
	kill_forks();
/*
    int parent_pid;
    scanf("%d",&parent_pid);
    kill_tree(parent_pid);
    printf("Saindo\n");
*/
}
