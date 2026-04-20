cmake_minimum_required(VERSION 3.16)
project(BankServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(SOURCES
    main.cpp
    server.cpp
    clienthandler.cpp
    bank.cpp
    user.cpp
    account.cpp
    savingsaccount.cpp
    checkingaccount.cpp
    userrepository.cpp
    accountrepository.cpp
    sessionmanager.cpp
)

set(HEADERS
    protocol.h
    transaction.h
    server.h
    clienthandler.h
    bank.h
    user.h
    account.h
    savingsaccount.h
    checkingaccount.h
    userrepository.h
    accountrepository.h
    sessionmanager.h
)

add_executable(BankServer ${SOURCES} ${HEADERS})

target_link_libraries(BankServer PRIVATE Qt6::Core Qt6::Network)
