/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageException.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 17:07:31 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/17 23:13:41 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <string>

class MessageException : public std::exception {
private:
	std::string message;
public:
	MessageException(std::string message);
	MessageException(char *c_message);
	virtual ~MessageException() throw();

	virtual const char	*what() const throw();
};