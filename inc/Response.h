/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 14:21:21 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/13 20:12:11 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_H
#define RESPONSE_H

#include "Request.h"
#include "server.h"

class Response {
   private:
    std::string _version;
    int _statusCode;
    std::string _statusMessage;
    std::map<std::string, std::string> _headers;
    std::string _body;
    bool isfinished;
    std::string returnval;
    size_t ind;
    bool final;

   public:
    Response();
    Response(std::string ver);
    ~Response() {};
    static std::string getStatusMessage(int code);
    void setStatus(int code);
    void setHeader(const std::string &key, const std::string &value);
    void setBody(const std::string &body);
    void setBodyFromFile(const std::string &filePath);
    int getStatus();
    std::string toString() const;
    void done();
    bool isdone();
    void setRetVal(std::string retval);
    std::string getRetVal();
    void setIndex(size_t val);
    size_t getIndex();
    void setType(std::string file);
    bool isFinal() const;
    void setFinal(bool v);

    static std::string expectHeader(std::string httpVer, unsigned int code);
};

#endif