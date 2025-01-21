#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include "Client.hpp"

class ACommand{
protected:
    
public:
    ACommand( void );
    ACommand( const ACommand &src );
    ACommand &operator=( const ACommand &src );
    
    virtual ~ACommand();

    virtual void execute(Client &client) = 0;
    
};

#endif // ACOMMAND_HPP