# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aboyreau <aboyreau@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/21 21:38:16 by atu               #+#    #+#              #
#    Updated: 2024/06/25 23:20:04 by atu              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CXXFLAGS += -Wall -Wextra -Werror -std=c++98

SRC = main

SRCS = $(addsuffix .cpp,$(SRC))
OBJS = $(addsuffix .o,$(SRC))

all: $(NAME)

$(NAME): $(LIBS) $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(NAME) $(LDLIBS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
