#include "../includes/minishell.h"

int	if_n(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == 'n')
			return (1);
		i++;
	}
	return (0);
}

int	if_nflag(char *str)
{
	int	i;
	int	flag;
	int	ret;

	ret = 1;
	flag = 0;
	i = 0;
	if (str[i] != '-' || !str[i])
		return (0);
	i = 1;
	if (if_n(str))
		flag = 1;
	while (str[i])
	{
		if (str[i] != 'n')
		{
			if ((str[i] == 'e' || str[i] == 'E') && flag)
				ret = 1;
			else
				return (0);
		}
		i++;
	}
	return (ret);
}

int	execute_echo(char **args)
{
	int	i;
	int	if_newline;

	if_newline = 1;
	i = 1;
	if (!args[1])
	{
		ft_putchar_fd('\n', 1);
		return (0);
	}
	while (args[i][0] == '-')
	{
		if (if_nflag(args[i]))
			if_newline = 0;
		else if (!if_n(args[i]))
			break ;
		i++;
	}
	execute_echo_utils(args, i);
	if (if_newline)
		ft_putchar_fd('\n', 1);
	return (0);
}

void	execute_echo_utils(char **args, int i)
{
	while (args[i])
	{
		ft_putstr_fd((char *)args[i], 1);
		if (args[i++ + 1])
			ft_putchar_fd(' ', 1);
	}
}
