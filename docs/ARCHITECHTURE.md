# System Architecture

## Overview

Fleet Manager is designed as a lightweight, high-performance distributed monitoring system.

## Components

### 1. Fleet Agent (C++)
- Lightweight agent deployed on edge devices.
- Collects metrics and reports to the backend.
- Exposes a local REST API for health checks and configuration.

### 2. Fleet Backend (C++)
- Centralized orchestrator.
- Manages device registrations, command dispatching, and data persistence (SQLite).
- Provides a RESTful API for the UI.

### 3. Fleet UI (Next.js)
- React-based dashboard.
- Real-time visualization of fleet status.

## Communication

- **Agent -> Backend:** Reports status and metrics via HTTP/REST.
- **UI -> Backend:** Management operations and status queries.
- **Backend -> Agent:** Control commands (e.g., via SSH or persistent connections).

## Tech Stack

- **Backend/Agent:** C++20, Crow (Web Framework), fmt, spdlog, vcpkg.
- **Frontend:** Next.js 15+, Tailwind CSS, TypeScript.
- **Deployment:** Docker, Docker Compose.
