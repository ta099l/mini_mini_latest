#include "../includes/minishell.h"

void ft_sort_str_array(char **array)
{
        int i;
        int j;
        char *temp;

        i = 0;
        temp = NULL;
        while (array[i] != NULL)
        {
                j = i + 1;
                while (array[j] != NULL)
                {
                        if (ft_strncmp(array[i], array[j], 4096) > 0)
                        {
                                temp = array[i];
                                array[i] = array[j];
                                array[j] = temp;
                        }
                        j++;
                }
                i++;
        }
}

void print_export(char **sorted_env, int counter)
{
        int i;
        char *key;
        char *value;

        i = 0;
        while (i < counter)
        {
                if (ft_strchr(sorted_env[i], '='))
                {
                        key = ft_substr(sorted_env[i], 0, ft_strchr(sorted_env[i], '=') - sorted_env[i]);
                        value = ft_strchr(sorted_env[i], '=') + 1;
                        printf("declare -x %s=\"%s\"\n", key, value);
                        free(key);
                }
                else
                        printf("declare -x %s\n", sorted_env[i++]);
        }
        i = 0;
        while (sorted_env[i])
        {
                free(sorted_env[i]);
                i++;
        }
        free(sorted_env);
}

void no_args(t_envp *env)
{
        char **sorted_envp;
        int i;

        i = 0;
        sorted_envp = malloc(sizeof(char *) * (env->counter + 1));
        if (!sorted_envp)
        {
                perror("malloc");
                return;
        }
        while (i < env->counter)
        {
                sorted_envp[i] = ft_strdup(env->tmp_envp[i]);
                i++;
        }
        sorted_envp[i] = NULL;
        ft_sort_str_array(sorted_envp);
        print_export(sorted_envp, env->counter);
}

int execute_export(char **args, t_envp *env)
{
        int i;
        char *equal_sign;

        i = 1;
        if (args[1] == NULL)
                no_args(env);
        else
        {
                while (args[i])
                {
                        equal_sign = ft_strchr(args[i], '=');
                        if (equal_sign)
                                if_true(args, env, equal_sign, i);
                        else
                                if_not(args, env, i);
                        i++;
                }
        }
        return (0);
}