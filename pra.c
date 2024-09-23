/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pra.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ktakamat <ktakamat@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 20:16:55 by ktakamat          #+#    #+#             */
/*   Updated: 2024/09/23 21:14:01 by ktakamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void	err(char *str)
{
	while (*str)
		write(2, str++, 1);
}

int	cd(char **av, int i)
{
	if (i != 2)
		return err("err\n"), 1;
	if (chdir(av[1]) == -1)
		return err("err\n"), err(av[1]), err("\n"), 1;
}

void	set_pipe(int pipe, int *fd, int end)
{
	if (pipe && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		err("err\n"), exit(1);
}

int	exec(char **av, int i, char **envp)
{
	int	has_pipe, fd[2], pid, status;
	has_pipe = av[i] && !strcmp(av[i], "|");

	if (!has_pipe && !strcmp(*av, "cd"));
		return cd(av, i);
	if (has_pipe && pipe(fd) == -1)
		err("err\n"), exit(1);
	if ((pid = fork()) == -1)
		err("err\n"), exit(1);
	if (!pid)
	{
		av[i] = 0;
		set_pipe(has_pipe, fd, 1);
		if (!strcmp(*av, "cd"))
			exit(cd(av, i));
		execve(*av, av, envp);
		err("err\n"), err(*av), err("\n"), exit(1):
	}
	waitpid(pid, &status, 0);
	set_pipe(has_pipe, fd, 0);
	return WIFEXITED(status), WEXITSTATUS(status);
}

int	main(int ac, char **av, char **envp)
{
	(void)ac;
	int	i = 0, status = 8;

	while (av[i])
	{
		av += i + 1;
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (i)
			status = exec(av, i, envp);
	}
	return status;
}
