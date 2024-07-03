---
title: ConnectionProperties
description:  This class is a container for various properties related to a connection with a Polypheny instance independent of the type of transport used.
generator: doxide
---


# ConnectionProperties

**class ConnectionProperties**


This class is a container for various properties related to a connection with a Polypheny instance independent of the type of transport used.
Transports refer to the way the driver communicates with the Polypheny instance. Currently plain TCP and plain UNIX sockets are supported. Their options can be configured in the corresponding classes.
Connection properties on the other hand only deal with user preferences and authentication. Parameters like username, password or options like autocommit are handled.
Connection properties can be reused. Modifications made to the properties don't affect connections already established using this properties object..
    


## Functions

| Name | Description |
| ---- | ----------- |
| [ConnectionProperties](#ConnectionProperties) | Constructs a ConnectionProperties object with optional parameters. * Default values are used for the omitted parameters. Setters can be used to modify parameters later on. |
| [get_username](#get_username) | Gets the username to be used on connection. |
| [set_username](#set_username) | Sets the username to be used for authentication. |
| [set_is_password_required](#set_is_password_required) | Sets whether a password is used for authentication. |
| [get_is_password_required](#get_is_password_required) | Gets whether a password should be used for authentication. |
| [get_password](#get_password) | Gets the password that will be used for the connection. |
| [set_password](#set_password) | Sets the password to be used for authentication. |
| [get_default_namespace](#get_default_namespace) | Gets the default namespace to be used for the connection. |
| [set_default_namespace](#set_default_namespace) | Sets the default namespace to be used for the connection. |
| [get_is_auto_commit](#get_is_auto_commit) | Gets whether auto-commit should be enabled. |
| [set_is_auto_commit](#set_is_auto_commit) | Sets whether auto-commit should be enabled enabled. |

## Function Details

### ConnectionProperties<a name="ConnectionProperties"></a>
!!! function "explicit ConnectionProperties(std::string username = &quot;pa&quot;, std::string password = &quot;&quot;, std::string default_nspace = &quot;public&quot;, bool is_password_required = true, bool is_auto_commit = true)"

    
    Constructs a ConnectionProperties object with optional parameters.
             * Default values are used for the omitted parameters. Setters can be used to modify parameters later on.
    
    
    :material-location-enter: **Parameter** `username`
    :    The username for authentication (default is "pa").
        
    :material-location-enter: **Parameter** `password`
    :    The password for authentication (default is an empty string).
        
    :material-location-enter: **Parameter** `default_nspace`
    :    The default namespace for the connection (default is "public").
        
    :material-location-enter: **Parameter** `is_password_required`
    :    Indicates if a password is required (default is true).
        
    :material-location-enter: **Parameter** `is_auto_commit`
    :    Indicates if auto-commit is enabled (default is true).
                
    

### get_default_namespace<a name="get_default_namespace"></a>
!!! function "[[nodiscard]] std::string get_default_namespace() const"

    
    Gets the default namespace to be used for the connection.
    
    
    :material-keyboard-return: **Return**
    :    The default namespace.
            
    

### get_is_auto_commit<a name="get_is_auto_commit"></a>
!!! function "[[nodiscard]] bool get_is_auto_commit() const"

    
    Gets whether auto-commit should be enabled.
    
    
    :material-keyboard-return: **Return**
    :    True if auto-commit is enabled, false otherwise.
            
    

### get_is_password_required<a name="get_is_password_required"></a>
!!! function "[[nodiscard]] bool get_is_password_required() const"

    
    Gets whether a password should be used for authentication.
    
    
    :material-keyboard-return: **Return**
    :    True if a password is required, false otherwise.
              
    

### get_password<a name="get_password"></a>
!!! function "[[nodiscard]] std::string get_password() const"

    
    Gets the password that will be used for the connection.
    
    
    :material-keyboard-return: **Return**
    :    The password.
            
    

### get_username<a name="get_username"></a>
!!! function "[[nodiscard]] std::string get_username() const"

    
    Gets the username to be used on connection.
    
    
    :material-keyboard-return: **Return**
    :    The username.
            
    

### set_default_namespace<a name="set_default_namespace"></a>
!!! function "void set_default_namespace(const std::string &amp;default_nspace)"

    
    Sets the default namespace to be used for the connection.
    
    
    :material-location-enter: **Parameter** `default_nspace`
    :    The new default namespace.
                
    

### set_is_auto_commit<a name="set_is_auto_commit"></a>
!!! function "void set_is_auto_commit(bool is_auto_commit)"

    
    Sets whether auto-commit should be enabled enabled.
    
    
    :material-location-enter: **Parameter** `is_auto_commit`
    :    True to enable auto-commit, false to disable it.
                
    

### set_is_password_required<a name="set_is_password_required"></a>
!!! function "void set_is_password_required(bool is_password_required)"

    
    Sets whether a password is used for authentication.
    
    If this is set to false on a connection requiring a password an error will be thrown when the connection is established.
    
    
    :material-location-enter: **Parameter** `is_password_required`
    :    True if a password is required, false otherwise.
                
    

### set_password<a name="set_password"></a>
!!! function "void set_password(const std::string &amp;password)"

    
    Sets the password to be used for authentication.
    
    If "is_password_required" is set to false, this value will be ignored.
    
    
    :material-location-enter: **Parameter** `password`
    :    The new password.
                
    

### set_username<a name="set_username"></a>
!!! function "void set_username(const std::string &amp;username)"

    
    Sets the username to be used for authentication.
    
    
    :material-location-enter: **Parameter** `username`
    :    The new username.
                
    

