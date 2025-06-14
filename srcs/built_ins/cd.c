#include "../includes/minishell.h"

static int	update_pwd(char *old_pwd, t_envp *env, int *status)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		*status = 1;
		return (error_mess(old_pwd, NULL, "cd: getcwd"));
	}
	add_or_update_env(env, "OLDPWD", old_pwd);
	add_or_update_env(env, "PWD", new_pwd);
	return (error_mess(old_pwd, new_pwd, NULL));
}

int	execute_cd(char **args, t_envp *env, int *status)
{
	char	*tar_dir;
	char	*old_pwd;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
	{
		*status = 1;
		return (error_mess(NULL, NULL, "cd: getcwd"));
	}
	tar_dir = execute_cd_utils(args, env);
	if (!tar_dir || *tar_dir == '\0')
	{
		write(2, "cd: target not set\n", 19);
		free(old_pwd);
		return (1);
	}
	if (chdir(tar_dir) != 0)
	{
		*status = 1;
		return (error_mess(old_pwd, NULL, "cd"));
	}
	return (update_pwd(old_pwd, env, status));
}

char	*execute_cd_utils(char **args, t_envp *env)
{
	char	*tar_dir;

	if (!args[0])
		tar_dir = ft_getenv("HOME", env);
	else if (args[0] && ft_strncmp(args[0], "-", 1) == 0)
		tar_dir = ft_getenv("OLDPWD", env);
	else
		tar_dir = args[0];
	return (tar_dir);
}

int	error_mess(char *old_pwd, char *new_pwd, char *mess)
{
	if (mess)
		perror(mess);
	else
	{
		free(old_pwd);
		free(new_pwd);
		return (0);
	}
	if (old_pwd)
		free(old_pwd);
	return (1);
}
