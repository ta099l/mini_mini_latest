#ifndef MINISHELL_H
#define MINISHELL_H

// Libraries
#include "../libft/libft.h"
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Data Structures

typedef struct s_envp
{
    char **tmp_envp;
    int counter;
} t_envp;

typedef enum e_types
{
    WORD = 1,
    PIPE = 2,
    REDIR = 3,
    HEREDOC = 4,
} t_types;

typedef struct s_tmptoken
{
    char *value;
    int start;
    int end;
} t_tmptoken;

typedef struct s_token
{
    char *value;
    char quotes;
    t_types type;
    struct s_token *prev;
    struct s_token *next;
} t_token;

typedef struct s_command
{
    char **args;
    char *infile;
    char *outfile;
    int append;
    int heredoc;
    int executable;
    struct s_command *next;
} t_command;

typedef struct s_all
{
    struct s_token *token;
    struct s_tmptoken *tmp;
    struct s_command *cmd;
    struct s_envp *cp_envp;
    pid_t *child_pids;
    int exit_status;
} t_all;

// Tokenization & Parsing
t_token *find_last_node(t_token *token);
int add_node(t_token **token, char *input);
int toknize(char *input, t_all *as);
int split_input(t_all *as, char *input, t_token **token, t_tmptoken *tmp);
void token_types(t_token *token);
int scan_token(char *input, int i, t_tmptoken *tmp);
int str(t_all *as, char *input, int i, t_tmptoken *tmp);
int is_parameter(char c);
int parameter_token(t_all *as, char *input, int i, t_tmptoken *tmp);

// Init
t_all *init_strcuts(t_all *as);
void init_token(t_tmptoken *tmptoken);

// Environment
void copy_envp(char **envp, t_all *as);
void print_envp(t_envp *cp_envp);
char *ft_getenv(const char *name, t_envp *cp_envp);
void add_or_update_env(t_envp *env, char *key, char *value);
void update_env_var(t_envp *env, const char *key, const char *value);
void remove_env_var(t_envp *env, const char *key);
void remove_env_var_utils(t_envp *env, char **new_env, int j);

// Expander
int expand_var(t_all *as, t_token *token, t_envp *cp_envp, int heredoc_expander);
int expand_variable(t_token *token, char *env_value, int start, int end);
int process_token(t_all *as, t_token *token, t_envp *cp_envp);
int process_dollar(t_all *as, t_token *token, int *i, t_envp *cp_envp);
char *join_before_env(t_token *token, char *env_value, int start);
int join_after_and_replace(t_token *token, char *var_value, int end, int len);
int extract_variable(t_all *as, char *value, int *i, char **var);
int handle_exit_status(t_all *as, t_token *token);

// Quotes
int closing_qoutes(char *input, int i);
int find_closing_quote(char *input, int i, int flag, char quote_type);
int handle_quotes(t_all *as, char *input, int i, t_tmptoken *tmp);
void replace_qoutes(t_token *token, int i, int j, int q);
void remove_quotes(t_token *token);
int handle_single_quotes(const char *value, int *i, int in_double);
int toggle_double_quote(int ch, int in_double);
int check_qout(char *value);

// Commands
t_command *new_command(t_all *as);
int split_cmds(t_all *as, t_token *token, t_command **cmd_list);
char *find_path(t_envp *cp_envp, char *cmd);
char *get_full_path(char *dir, char *cmd);
void execute_commands(t_all *as, t_command *cmd_list, t_envp *env);

// Built-ins
int built_in(t_command *cmd_list);
void execute_built_ins(t_command *cmd_list, t_envp *env, t_all *as);
int execute_echo(char **args);
void execute_echo_utils(char **args, int i);
int execute_cd(char **args, t_envp *env);
char *execute_cd_utils(char **args, t_envp *env);
int execute_env(char **args, t_envp *env);
int execute_export(char **args, t_envp *env);
int execute_pwd(char **args);
int execute_unset(char **args, t_envp *env);
int error_mess(char *old_pwd, char *new_pwd, char *mess);
void print_export(char **sorted_env, int counter);
void no_args(t_envp *env);
int is_valid_key(char *key);
int is_valid_unset_key(char *key);
void ft_sort_str_array(char **array);
void if_true(char **args, t_envp *env, char *equal_sign, int i);
void if_not(char **args, t_envp *env, int i);
// Memory & Cleanup
void free_all(t_all *as);
void free_tokens(t_token *token);
void free_args(char **args);
void free_files(t_command *cmd);
int free_envp(t_envp *cp_envp);
int free_token_cmd(t_all *as);
int clean(t_all *as);
int clean_all(t_all *as);
void free_exit_status(t_all *as);
void token_error(t_token *token, char *msg);

// Execution Helpers
char **add_arg(t_all *as, char **args, char *value);
char *cur_dir(t_all *as);
char *heredoc_cmd(t_all *as, char *del, int n);

// Signals
void sig_handler_prompt(int signum);
void setup_signals(void);
void restore_signals(void);
void ignore_signals(void);

// Exit
void exit_program(t_all *as, char *str, int n);
void exit_fork(t_all *as, char *str);
void exit_forkk(t_all *as, char *str, int n);

// Shell entry
void start_shell(t_all *as);
void setup_environment(int argc, char **argv, char **envp, t_all **as);

// Debug
void print_list(t_token *head);
void print_commands(t_command *cmd_list);

#endif // MINISHELL_H