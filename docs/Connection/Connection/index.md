---
title: Connection
description: Represents a connection to the Polypheny database.
generator: doxide
---


# Connection

**class Connection**


Represents a connection to the Polypheny database.

This class represents a connection a Polypheny instance. It further provides all methods required to establish such a connection.
Methods are provided to establish a connection based on defaults or a custom configuration.
    


## Functions

| Name | Description |
| ---- | ----------- |
| [Connection](#Connection) | Constructs a Connection object with the given connection properties. |
| [~ Connection](#_u007eConnection) | Destroys the Connection object thus closing the connection.  |
| [Connection](#Connection) | Constructs a Connection object using the specified host, user, and password. |
| [Connection](#Connection) | Constructs a Connection object with the given connection properties and transport. |

## Function Details

### Connection<a name="Connection"></a>
!!! function "explicit Connection(ConnectionProperties &amp;connection_properties)"

    
    Constructs a Connection object with the given connection properties.
    
    
    :material-location-enter: **Parameter** `connection_properties`
    :    The properties used to configure the connection.
                
    

!!! function "Connection(const std::string &amp;host, const std::string &amp;user, const std::string &amp;password)"

    
    Constructs a Connection object using the specified host, user, and password.
    
    
    :material-location-enter: **Parameter** `host`
    :    The hostname or IP address of the Polypheny instance.
        
    :material-location-enter: **Parameter** `user`
    :    The username of the Polypheny user for which this connection should be opened.
        
    :material-location-enter: **Parameter** `password`
    :    The password of the above Polypheny user for authentication.
                
    

!!! function "Connection(ConnectionProperties&amp; connection_properties, std::unique_ptr&lt;Transport::Transport&gt; &amp;&amp;transport)"

    
    Constructs a Connection object with the given connection properties and transport.
    
    
    :material-location-enter: **Parameter** `connection_properties`
    :    The properties used to configure the connection.
        
    :material-location-enter: **Parameter** `transport`
    :    A unique pointer to a transport object used for communication with the Polypheny instance.
                
    

### ~ Connection<a name="_u007eConnection"></a>
!!! function "~ Connection()"

    
    Destroys the Connection object thus closing the connection.
             
    
    
    

