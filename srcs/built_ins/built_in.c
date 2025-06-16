/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tabuayya <tabuayya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 16:55:38 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/16 09:34:29 by tabuayya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	built_in(t_command *cmd_list)
{
	t_command	*cmd;
	int			return_va;

	cmd = cmd_list;
	return_va = 0;
	if (!cmd->args)
		return (0);
	if (ft_strncmp(cmd->args[0], "echo", 4) == 0)
		return_va = 1;
	else if (ft_strncmp(cmd->args[0], "cd", 2) == 0)
		return_va = 1;
	else if (ft_strncmp(cmd->args[0], "pwd", 3) == 0)
		return_va = 1;
	else if (ft_strncmp(cmd->args[0], "export", 6) == 0)
		return_va = 1;
	else if (ft_strncmp(cmd->args[0], "exit", 4) == 0)
		return_va = 1;
	else if (ft_strncmp(cmd->args[0], "unset", 5) == 0)
		return_va = 1;
	else if (ft_strncmp(cmd->args[0], "env", 3) == 0)
		return_va = 1;
	return (return_va);
}

void	execute_exit(char **args, t_all *as, int *status)
{
	int	exit_status;

	exit_status = 0;
	printf("exit\n");
	if (args[1] && !ft_isalpha(args[1][0]) && args[2])
	{
		ft_putstr_fd("exit: too many arguments\n", 2);
		*status = 1;
		return ;
	}
	if (args[1])
		execute_exit_utils(args, as, exit_status);
	else
	{
		clean(as);
		exit(0);
	}
}

void	execute_exit_utils(char **args, t_all *as, int exit_status)
{
	int	i;

	i = 0;
	while (args[1][i])
	{
		if (!ft_isdigit(args[1][i]) && args[1][i] != '-' && args[1][i] != '+')
		{
			ft_putstr_fd("exit: numeric argument required\n", 2);
			clean(as);
			exit(2);
		}
		i++;
	}
	exit_status = ft_atoi(args[1]);
	clean(as);
	exit(exit_status);
}

void	execute_built_ins(t_command *cmd_list, t_envp *env, t_all *as)
{
	t_command	*cmd;
	int			status;

	status = 0;
	cmd = cmd_list;
	if (cmd)
	{
		if (ft_strncmp(cmd->args[0], "echo", 4) == 0)
			execute_echo(cmd->args);
		else if (ft_strncmp(cmd->args[0], "cd", 2) == 0)
			execute_cd(&cmd->args[1], env, &status);
		else if (ft_strncmp(cmd->args[0], "pwd", 3) == 0)
			execute_pwd(&cmd->args[1]);
		else if (ft_strncmp(cmd->args[0], "export", 6) == 0)
			execute_export(cmd->args, env, &status);
		else if (ft_strncmp(cmd->args[0], "unset", 5) == 0)
			execute_unset(cmd->args, env);
		else if (ft_strncmp(cmd->args[0], "env", 3) == 0)
			execute_env(cmd->args, env);
		else if (ft_strncmp(cmd->args[0], "exit", 4) == 0)
			execute_exit(cmd->args, as, &status);
	}
	as->exit_status = status;
}
