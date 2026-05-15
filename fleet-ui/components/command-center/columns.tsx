"use client";

import { ColumnDef } from "@tanstack/react-table";
import { Node } from "@/types";

export const columns: ColumnDef<Node>[] = [
    {
        id: "select",
        header: ({ table }) => (
            <input
                type="checkbox"
                checked={table.getIsAllPageRowsSelected()}
                onChange={table.getToggleAllPageRowsSelectedHandler()}
            />
        ),
        cell: ({ row }) => (
            <input
                type="checkbox"
                checked={row.getIsSelected()}
                disabled={!row.getCanSelect()}
                onChange={row.getToggleSelectedHandler()}
            />
        ),
        enableSorting: false,
        enableHiding: false,
    },
    {
        accessorKey: "hostname",
        header: "Hostname",
    },
    {
        accessorKey: "ip",
        header: "IP Address",
    },
    {
        accessorKey: "status",
        header: "Status",
    },
    {
        accessorKey: "os",
        header: "Operating System",
    },
    {
        accessorKey: "agent_version",
        header: "Agent Version",
    },
];
