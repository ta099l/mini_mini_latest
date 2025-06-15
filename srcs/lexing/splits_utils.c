#include "../includes/minishell.h"

int	toknize(char *input, t_all *as)
{
	int	i;

	as->error_flag = 0;
	i = split_input(as, input, &as->token, as->tmp);
	if (i == -1)
		return (free_exit_status(as), 1);
	i = expand_var(as, as->token, as->cp_envp, 0);
	if(i == -1)
		exit_program(as, "malloc failed", 1);
	remove_quotes(as->token);
	split_cmds(as, as->token, &as->cmd);
	if (!as->error_flag)
		execute_commands(as, as->cmd, as->cp_envp);
	return (0);
}

void	token_types(t_token *token)
{
	while (token)
	{
		if (ft_strncmp(token->value, "<<", 2) == 0)
			token->type = HEREDOC;
		else if ((ft_strncmp(token->value, ">>", 2) == 0)
			|| (ft_strncmp(token->value, ">", 1) == 0)
			|| (ft_strncmp(token->value, "<", 1) == 0))
		{
			token->type = REDIR;
		}
		else if (ft_strncmp(token->value, "|", 1) == 0)
			token->type = PIPE;
		else
			token->type = WORD;
		token = token->next;
	}
}

int	parameter_token(t_all *as, char *input, int i, t_tmptoken *tmp)
{
	tmp->start = i;
	if (input[i] && input[i + 1] && input[i] == input[i + 1]
		&& input[i + 1] != '|')
		i++;
	tmp->end = i;
	tmp->value = ft_substr(input, tmp->start, (tmp->end - tmp->start + 1));
	if (!tmp->value)
	{
		exit_program(as, "Memory allocation failed", 1);
	}
	if (add_node(&as->token, tmp->value) == -1)
	{
		return (-1);
	}
	free(tmp->value);
	tmp->value = NULL;
	return (i);
}

int	is_parameter(char c)
{
	if (c == '|' || c == '>' || c == '<')
	{
		return (1);
	}
	return (0);
}
