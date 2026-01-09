# Online Airline Ticket Booking System



---

## Authors
- **Vu Hai Dang** – 20225962  
- **Nguyen Minh Khoi** – 20226050  

---

## Background and Motivation

The rapid development of information technology and computer networks has fundamentally transformed the way modern services are delivered, particularly in the transportation and aviation sectors. Traditional airline ticket booking methods, which relied heavily on physical counters and manual processing, have gradually been replaced by online booking platforms that offer convenience, speed, and real-time access to flight information.

From an academic standpoint, online airline booking systems represent a typical class of distributed applications that integrate networking, data management, and user interaction. Such systems require reliable communication between clients and servers, efficient handling of concurrent requests, secure user authentication, and consistent transaction processing. Consequently, they provide an ideal context for applying theoretical knowledge from Computer Networks and Software Engineering to a practical, real-world scenario.

Motivated by these considerations, this project focuses on the design and implementation of an **Online Airline Ticket Booking Application**. The project emphasizes the use of low-level **socket-based client–server communication** to illustrate how network protocols operate in practice, while simultaneously addressing realistic functional requirements such as flight search, ticket booking, payment processing, and notification services.

---

## Project Objectives

The primary objective of this project is to develop a fully functional client–server application that simulates an online airline ticket booking system. The system is designed to satisfy both technical and functional requirements specified in the assignment, thereby ensuring completeness and practical relevance.

The key objectives include:

- **User Authentication**  
  Provide secure user registration and login mechanisms.

- **Flight Search**  
  Enable users to search for flights based on multiple criteria such as departure location, destination, travel dates, number of passengers, and seat class.

- **Flight Comparison**  
  Allow users to compare ticket prices and flight durations across different airlines.

- **Ticket Booking and Payment**  
  Implement a complete booking workflow including seat reservation and online payment processing.

- **Electronic Ticket Delivery**  
  Automatically generate and deliver electronic tickets to users via email or messaging services.

- **Ticket Management**  
  Support viewing booking details, canceling tickets, modifying bookings, and printing electronic tickets.

- **Flight Notifications**  
  Notify users of flight-related events such as schedule changes, delays, or cancellations.

---

## Scope and Technical Approach

This project is implemented with a strong focus on fundamental networking concepts. The scope and technical approach are defined as follows:

1. **Client–Server Communication**  
   Communication between the client and server is established using **TCP socket programming**, ensuring reliable data transmission and compliance with low-level networking requirements.

2. **Server Logging Mechanism**  
   The server records all incoming and outgoing messages in log files to support debugging, monitoring, and traceability.

3. **Concurrent Server Design**  
   The server is capable of handling multiple client connections simultaneously, reflecting real-world usage scenarios.

4. **Data Management**  
   Persistent storage is used to manage user accounts, flight information, booking records, and payment status in a structured and consistent manner.

5. **User Interface**  
   A graphical user interface (GUI) is developed to enhance usability and provide an intuitive interaction layer between users and the underlying network services.

---

## System Architecture Overview

The system follows a **Client–Server architecture**:
- The **Client** handles user interaction, request generation, and result presentation.
- The **Server** processes business logic, manages data storage, handles concurrent connections, and enforces security and consistency constraints.

Communication between the client and server is performed via TCP sockets using a custom application-layer protocol.

---

## Technologies Used

- Programming Language: **C**
- Networking: **TCP Socket Programming**
- Architecture: **Client–Server Model**
- Database: Relational database for persistent storage
- User Interface: Graphical User Interface (GUI)
- External Services: Email service for electronic ticket delivery

---

## Conclusion

Through the integration of socket-based communication, concurrent server design, structured data management, and user-centered interface development, this project demonstrates how theoretical concepts in computer networks and software engineering can be applied to build a practical, scalable, and feature-rich online airline ticket booking system.
