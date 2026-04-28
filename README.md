# Fleet Manager

A distributed system for managing and monitoring a fleet of devices.

## Project Structure

- `fleet-backend/`: C++ based central server managing state and APIs.
- `fleet-agent/`: C++ based agent to be installed on managed devices.
- `fleet-ui/`: Next.js frontend for monitoring and management.
- `docs/`: System architecture and design documentation.

## Getting Started

### Prerequisites

- Docker and Docker Compose
- C++20 Compiler (for local dev)
- CMake 3.25+
- vcpkg (for C++ dependency management)
- Node.js & Bun (for UI dev)

### Running with Docker

```bash
docker-compose up --build
```

### Component URLs

- UI: `http://localhost:3000`
- Backend API: `http://localhost:8080/api/health`
- Agent API: `http://localhost:8081/health`
