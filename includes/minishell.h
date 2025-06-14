/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* minishell.h                                        :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: combined <marvin@42.fr>                     +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/06/13 by combined           #+#    #+#             */
/* Updated: 2025/06/13 by combined           ###   ########.fr       */
/* */
/* ************************************************************************** */

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
    int export_falg;
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

typedef struct s_exec_ctx
{
    t_all	*as;
    t_envp	*env;
    int		*prev_fd;
    int		*fd;
    int		*num_forked_children;
    int		*max_children_capacity;
    int		c;
}	t_exec_ctx;

typedef struct s_child_ctx
{
    t_all		*as;
    t_command	*cmd;
    t_envp		*env;
    int			prev_fd;
    int			*fd;
}	t_child_ctx;

// Tokenization & Parsing
t_token *find_last_node(t_token *token);
int add_node(t_token **token, char *input);
int toknize(char *input, t_all *as);
int split_input(t_all *as, char *input, t_token **token, t_tmptoken *tmp);
int is_parameter(char c);
int parameter_token(t_all *as, char *input, int i, t_tmptoken *tmp);
int scan_token(char *input, int i, t_tmptoken *tmp);
int str(t_all *as, char *input, int i, t_tmptoken *tmp);
void token_types(t_token *token);

// Init
t_all *init_strcuts(t_all *as);
void init_token(t_tmptoken *tmptoken);

// Environment
void copy_envp(char **envp, t_all *as);
char *ft_getenv(const char *name, t_envp *cp_envp);
void add_or_update_env(t_envp *env, char *key, char *value);
void remove_env_var(t_envp *env, const char *key);
void remove_env_var_utils(t_envp *env, char **new_env, int j);
char *search_path_dirs(char *path, char *cmd);

// Expander
int expand_var(t_all *as, t_token *token, t_envp *cp_envp, int heredoc_expander);
int process_token(t_all *as, t_token *token, t_envp *cp_envp);
int process_dollar(t_all *as, t_token *token, int *i, t_envp *cp_envp);
char *join_before_env(t_token *token, char *env_value, int start);
int join_after_and_replace(t_token *token, char *var_value, int end, int len);
int expand_variable(t_token *token, char *env_value, int start, int end);
int extract_variable(t_all *as, char *value, int *i, char **var);
int handle_exit_status(t_all *as, t_token *token, int *i);
int handle_special_dollar(t_all *as, t_token *token, int *i);
int process_token_heredoc(t_all *as, t_token *token, t_envp *cp_envp);

// Quotes
int is_quoted(t_token *token);
int closing_qoutes(char *input, int i);
int find_closing_quote(char *input, int i, int flag, char quote_type);
int handle_quotes(t_all *as, char *input, int i, t_tmptoken *tmp);
void replace_qoutes(t_token *token, int i, int j, int q);
void remove_quotes(t_token *token);
int handle_single_quotes(const char *value, int *i, int in_double);
int toggle_double_quote(int ch, int in_double);

// Commands
t_command *new_command(t_all *as);
int split_cmds(t_all *as, t_token *token, t_command **cmd_list);
char *find_path(t_envp *cp_envp, char *cmd);
char *get_full_path(char *dir, char *cmd);
void execute_commands(t_all *as, t_command *cmd_list, t_envp *env);
void append_command(t_command **cmd_list, t_command *new_cmd);

// Built-ins
int built_in(t_command *cmd_list);
void execute_built_ins(t_command *cmd_list, t_envp *env, t_all *as);
void execute_exit(char **args, t_all *as, int *status);
void execute_exit_utils(char **args, t_all *as, int exit_status);
int execute_echo(char **args);
void execute_echo_utils(char **args, int i);
int if_n(char *str);
int if_nflag(char *str);
int execute_cd(char **args, t_envp *env, int *status);
char *execute_cd_utils(char **args, t_envp *env);
int execute_env(char **args, t_envp *env);
int execute_export(char **args, t_envp *env, int *status);
void end_update(t_envp *env, char *entry, char **new_env);
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
void free_cmd(t_command *cmd);
void free_tokens(t_token *token);
void free_args(char **args);
void free_files(t_command *cmd);
int free_envp(t_envp *cp_envp);
int free_token_cmd(t_all *as);
int clean(t_all *as);
void free_exit_status(t_all *as);

// Execution Helpers
char **add_arg(t_all *as, char **args, char *value);
char *cur_dir(t_all *as);
char *heredoc_cmd(t_all *as, char *del, int n, t_token *token);
int prepare_pipe_and_fork(t_all *as, int fd[2], int has_next);
void redirect_io(t_all *as, t_command *cmd, int prev_fd, int fd[2]);
void	child_process_logic_ctx(t_child_ctx *ctx);
void parent_process_cleanup(t_command *cmd, int *prev_fd, int fd[2]);

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

t_all *get_all_instance(t_all *set);

#endif // MINISHELL_H