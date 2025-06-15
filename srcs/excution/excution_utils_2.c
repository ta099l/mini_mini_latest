/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   excution_utils_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kabu-zee <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 16:57:59 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/15 16:58:02 by kabu-zee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	count_commands(t_command *cmd_list)
{
	int	count;

	count = 0;
	while (cmd_list)
	{
		count++;
		cmd_list = cmd_list->next;
	}
	return (count);
}

static void	handle_command(t_command *cmd_list, t_exec_ctx *ctx)
{
	if (built_in(cmd_list) && ctx->c == 1 && (!cmd_list->infile
			&& !cmd_list->outfile && !cmd_list->heredoc))
	{
		handle_builtin_parent(cmd_list, ctx);
	}
	else
	{
		handle_fork_and_exec(cmd_list, ctx);
	}
}

static void	loop_commands(t_command *cmd_list, t_exec_ctx *ctx)
{
	while (cmd_list)
	{
		if (cmd_list->executable == 1)
			handle_command(cmd_list, ctx);
		cmd_list = cmd_list->next;
	}
}

void	execute_commands(t_all *as, t_command *cmd_list, t_envp *env)
{
	int			fd[2];
	int			prev_fd;
	int			num_forked_children;
	int			max_children_capacity;
	t_exec_ctx	ctx;

	prev_fd = -1;
	as->child_pids = malloc(8 * sizeof(pid_t));
	if (!as->child_pids)
		exit_program(as, "malloc failed", 1);
	num_forked_children = 0;
	max_children_capacity = 8;
	ctx.c = count_commands(cmd_list);
	ctx.as = as;
	ctx.env = env;
	ctx.prev_fd = &prev_fd;
	ctx.fd = fd;
	ctx.num_forked_children = &num_forked_children;
	ctx.max_children_capacity = &max_children_capacity;
	loop_commands(cmd_list, &ctx);
	wait_for_children(as, num_forked_children);
}
