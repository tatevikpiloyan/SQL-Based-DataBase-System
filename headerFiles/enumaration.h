#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

// Enums
enum class Command
{
    INSERT = 1,
    SELECT,
    DELETE,
    UPDATE,
    MODIFY
};

enum class DBFile
{
    USER = 1,
    TASK,
    PROJECT
};

enum class User_Keyword 
{
    NAME = 1,
    SURNAME,
    POSITION,
    EMAIL,
    PHONE_NUMBER,
    WHERE,
    SET,
    ALL,
};

enum class Task_Keyword 
{
    NAME = 1,
    USER,
    CREATED,
    DEADLINE,
    WHERE,
    SET,
    ALL,
};

enum class Project_Keyword 
{
    NAME = 1,
    USER,
    TASK,
    CREATED,
    DEADLINE,
    DESCRIPTION,
    WHERE,
    SET,
    ALL,
    ADD,
    DELETE
};