#include "../includes/minishell.h"

// void	print_commands(t_command *cmd_list)
// {
// 	int			i;
// 	int			cmd_num;
// 	t_command	*cmd;

// 	return ;
// 	cmd_num = 1;
// 	cmd = cmd_list;
// 	while (cmd)
// 	{
// 		printf("=== Command %d ===\n", cmd_num);
// 		if (cmd->args)
// 		{
// 			printf("Arguments:\n");
// 			for (i = 0; cmd->args[i]; i++)
// 				printf("  [%d]: %s\n", i, cmd->args[i]);
// 		}
// 		else
// 			printf("Arguments: None\n");
// 		if (cmd->infile)
// 			printf("Infile: %s\n", cmd->infile);
// 		else
// 			printf("Infile: None\n");
// 		if (cmd->outfile)
// 			printf("Outfile: %s (append: %s)\n", cmd->outfile,
// 				cmd->append ? "yes" : "no");
// 		else
// 			printf("Outfile: None\n");
// 		if (cmd->heredoc)
// 			printf("Herdoc : %d", cmd->heredoc);
// 		printf("\n");
// 		cmd = cmd->next;
// 		cmd_num++;
// 	}
// }

t_command	*new_command(t_all *as)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		exit_program(as, "Memory allocation failed", 1);
	cmd->args = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->next = NULL;
	cmd->heredoc = 0;
	cmd->executable = 1;
	return (cmd);
}

char	**add_arg(t_all *as, char **args, char *value)
{
	char	**new_args;
	int		i;
	int		count;

	i = 0;
	count = 0;
	while (args && args[count])
		count++;
	new_args = malloc(sizeof(char *) * (count + 2));
	if (!new_args)
		exit_program(as, "Memory allocation failed", 1);
	while (i < count)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[count] = ft_strdup(value);
	new_args[count + 1] = NULL;
	if (args)
		free(args);
	return (new_args);
}

void	append_command(t_command **cmd_list, t_command *new_cmd)
{
	t_command	*tmp;

	if (!*cmd_list)
	{
		*cmd_list = new_cmd;
		return ;
	}
	tmp = *cmd_list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_cmd;
}

static int	handle_outfile(t_all *as, t_command *current_cmd, t_token *token,
		int *flag)
{
	int	fd_out;
	int	flags;

	if (ft_strncmp(token->value, ">>", 2) == 0)
		current_cmd->append = 1;
	if (current_cmd->outfile)
		free(current_cmd->outfile);
	current_cmd->outfile = ft_strdup(token->next->value);
	if (!*flag)
	{
		if (current_cmd->append)
			flags = O_WRONLY | O_CREAT | O_APPEND;
		else
			flags = O_WRONLY | O_CREAT | O_TRUNC;
		fd_out = open(current_cmd->outfile, flags, 0644);
		if (fd_out == -1)
		{
			perror("outfile");
			as->exit_status = 1;
			current_cmd->executable = 0;
			*flag = 1;
		}
		else
			close(fd_out);
	}
	return (0);
}

static int	handle_infile(t_all *as, t_command *current_cmd, t_token *token,
		int *flag)
{
	int	fd_in;

	if (current_cmd->infile)
		free(current_cmd->infile);
	current_cmd->infile = ft_strdup(token->next->value);
	if (!*flag)
	{
		fd_in = open(current_cmd->infile, O_RDONLY);
		if (fd_in == -1)
		{
			perror("infile");
			as->exit_status = 1;
			current_cmd->executable = 0;
			*flag = 1;
		}
		else
			close(fd_in);
	}
	return (0);
}

static int	handle_word_token(t_all *as, t_command *current_cmd, t_token *token)
{
	current_cmd->args = add_arg(as, current_cmd->args, token->value);
	if (!current_cmd->args)
		exit_program(as, "Memory allocation failed", 1);
	return (0);
}

static int	handle_heredoc_token(t_all *as, t_command *current_cmd,
		t_token **token)
{
	if (!(*token)->next || (*token)->next->type != WORD)
		exit_program(as, "Memory allocation failed", 1);
	current_cmd->heredoc = 1;
	if(current_cmd->infile )
	  free(current_cmd->infile );
	current_cmd->infile = heredoc_cmd(as, (*token)->next->value, *token);
	*token = (*token)->next;
	return (0);
}

static int	handle_redir_token(t_all *as, t_command *current_cmd,
		t_token **token, int *flag)
{
	if (!(*token)->next || (*token)->next->type != WORD)
		return (free_exit_status(as), 1);
	if (ft_strncmp((*token)->value, ">", 1) == 0)
		handle_outfile(as, current_cmd, *token, flag);
	else if (ft_strncmp((*token)->value, "<", 1) == 0)
		handle_infile(as, current_cmd, *token, flag);
	*token = (*token)->next;
	return (0);
}

int	split_cmds(t_all *as, t_token *token, t_command **cmd_list)
{
	t_command	*current_cmd;
	int			flag;

	*cmd_list = NULL;
	current_cmd = new_command(as);
	flag = 0;
	if (!current_cmd)
	{
		free(current_cmd);
		exit_program(as, "Memory allocation failed", 1);
	}
	while (token)
	{
		if (token->type == WORD)
			handle_word_token(as, current_cmd, token);
		else if (token->type == HEREDOC)
			handle_heredoc_token(as, current_cmd, &token);
		else if (token->type == REDIR)
			handle_redir_token(as, current_cmd, &token, &flag);
		else if (token->type == PIPE)
		{
			append_command(cmd_list, current_cmd);
			current_cmd = new_command(as);
			if (!current_cmd)
				exit_program(as, "Memory allocation failed", 1);
		}
		token = token->next;
	}
	append_command(cmd_list, current_cmd);
	return (0);
}
