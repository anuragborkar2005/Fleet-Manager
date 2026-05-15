export type Node = {
    id: string;
    hostname: string;
    ip: string;
    status: string;
    os: string;
    agent_version: string;
    cpu_usage?: number;
    ram_usage?: number;
    uptime?: number;
};
