#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_WORK_DIR 256
#define MAX_BG_PROCESSES 64

// Linked list node for separate chaining
typedef struct Node
{
	int key;
	struct Node *next;
} Node;

// Hash set structure
typedef struct
{
	Node *table[MAX_BG_PROCESSES];
} HashSet;

// Hash function
unsigned int hash(int key)
{
	return (unsigned int)key % MAX_BG_PROCESSES;
}

// Create a new set
HashSet *create_set()
{
	HashSet *set = (HashSet *)malloc(sizeof(HashSet));
	for (int i = 0; i < MAX_BG_PROCESSES; i++)
	{
		set->table[i] = NULL;
	}
	return set;
}

// Check if key exists
int contains(HashSet *set, int key)
{
	unsigned int idx = hash(key);
	Node *curr = set->table[idx];
	while (curr)
	{
		if (curr->key == key)
			return 1;
		curr = curr->next;
	}
	return 0;
}

// Insert key (no duplicates)
void insert(HashSet *set, int key)
{
	if (contains(set, key))
		return; // Skip if already exists
	unsigned int idx = hash(key);
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->key = key;
	newNode->next = set->table[idx];
	set->table[idx] = newNode;
}

// Remove key
void remove_key(HashSet *set, int key)
{
	unsigned int idx = hash(key);
	Node *curr = set->table[idx];
	Node *prev = NULL;

	while (curr)
	{
		if (curr->key == key)
		{
			if (prev)
				prev->next = curr->next;
			else
				set->table[idx] = curr->next;
			free(curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

// Free memory
void free_set(HashSet *set)
{
	for (int i = 0; i < MAX_BG_PROCESSES; i++)
	{
		Node *curr = set->table[i];
		while (curr)
		{
			Node *temp = curr;
			curr = curr->next;
			free(temp);
		}
	}
	free(set);
}

void kill_all_bg_processes(HashSet *set)
{
	for (int i = 0; i < MAX_BG_PROCESSES; i++)
	{
		Node *curr = set->table[i];
		while (curr)
		{
			if (kill(curr->key, 0) == 0) // check if still running
			{
				kill(curr->key, SIGTERM); // send terminate signal
			}
			curr = curr->next;
		}
	}
}

void handler(int sigint)
{
}

/* Splits the string by space and returns the array of tokens
 *
 */
char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++)
	{

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t')
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0)
			{
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else
		{
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}

int main(int argc, char *argv[])
{

	// signal(SIGINT, handler);
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;

	HashSet *bg_ps = create_set();
	int no_ps = 0;

	while (1)
	{
		/* BEGIN: TAKING INPUT */
		memset(line, 0, sizeof(line));
		char buf[MAX_WORK_DIR];
		getcwd(buf, MAX_WORK_DIR);
		printf("%s ", buf);
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		int status;
		int temp;
		while ((temp = waitpid(-1, &status, WNOHANG)) > 0)
		{
			remove_key(bg_ps, temp);
			no_ps--;
			printf("Background process with PID %d exited with status %d\n",
				   temp, WEXITSTATUS(status));
			fflush(stdout);
		}

		/* END: TAKING INPUT */

		if (strlen(line) == 0)
		{
			continue;
		}

		line[strlen(line)] = '\n';
		tokens = tokenize(line);

		if (strcmp(tokens[0], "cd") == 0)
		{
			if (chdir(tokens[1]))
			{
				perror("Failure: ");
			}
			continue;
		}

		if (strcmp(tokens[0], "exit") == 0)
		{
			// Freeing the allocated memory
			for (i = 0; tokens[i] != NULL; i++)
			{
				free(tokens[i]);
			}
			free(tokens);

			kill_all_bg_processes(bg_ps);
			while ((temp = waitpid(-1, &status, 0)) > 0)
			{
				printf("Background process with PID %d exited with status %d\n",
					   temp, WEXITSTATUS(status));
				fflush(stdout);
			}
			sleep(100);
			free_set(bg_ps);
			break;
		}

		int InBackground = 0;

		int i = 0;
		while (tokens[i] != NULL)
			i++;

		if (i > 0 && strcmp(tokens[i - 1], "&") == 0)
		{
			tokens[i - 1] = NULL;
			InBackground = 1;
		}

		pid_t pid = fork();
		if (pid < 0)
		{
			perror("Fork failed");
			exit(1);
		}
		else if (pid == 0)
		{
			if (!InBackground)
			{
				signal(SIGINT, SIG_DFL);
			}
			else
			{
				setpgid(0, 0);
			}

			execvp(tokens[0], tokens);
			fflush(stdout);
			perror("execvp failed");
			exit(1);
		}
		else
		{
			int status;
			if (!InBackground)
			{
				int ret = waitpid(pid, &status, 0);
				if (ret > 0 && status != 0)
				{
					printf("EXITSTATUS: %d\n", WEXITSTATUS(status));
				}
			}
			else
			{
				insert(bg_ps, pid);
				no_ps++;
			}
		}

		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++)
		{
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}
