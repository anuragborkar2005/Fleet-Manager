"use client";

import {
  ChartBarIncreasingIcon,
  CommandIcon,
  LayoutDashboard,
  Server,
  ServerIcon,
  Shell,
} from "lucide-react";
import React from "react";
import {
  Sidebar,
  SidebarContent,
  SidebarFooter,
  SidebarHeader,
  SidebarMenu,
  SidebarMenuButton,
  SidebarMenuItem,
  SidebarRail,
  SidebarSeparator,
} from "./ui/sidebar";
import Link from "next/link";

const nav_items = {
  user: {
    name: "Guest",
    email: "guest@gmail.com",
    avatar: "",
  },
  items: [
    {
      name: "Overview",
      path: "/dashboard/overview",
      icon: LayoutDashboard,
    },
    {
      name: "Analytics",
      path: "/dashboard/analytics",
      icon: ChartBarIncreasingIcon,
    },
    {
      name: "Command Center",
      path: "/dashboard/command",
      icon: CommandIcon,
    },
    {
      name: "Shell Terminal",
      path: "/dashboard/shell",
      icon: Shell,
    },
  ],
};

export function AppSidebar({ ...props }: React.ComponentProps<typeof Sidebar>) {
  return (
    <Sidebar collapsible="icon" {...props}>
      <SidebarHeader className="h-12">
        <SidebarMenu>
          <SidebarMenuItem>
            <SidebarMenuButton asChild>
              <Link href="/">
                <ServerIcon />
                <span>Fleet Manager</span>
              </Link>
            </SidebarMenuButton>
          </SidebarMenuItem>
        </SidebarMenu>
      </SidebarHeader>
      <SidebarSeparator />
      <SidebarContent>
        <SidebarMenu>
          {nav_items.items.map((item) => (
            <SidebarMenuItem key={item.name} className="ml-2">
              <SidebarMenuButton asChild>
                <Link href={item.path} className="font-medium">
                  <item.icon />
                  {item.name}
                </Link>
              </SidebarMenuButton>
            </SidebarMenuItem>
          ))}
        </SidebarMenu>
      </SidebarContent>
      <SidebarFooter>
        <SidebarMenu>
          <SidebarMenuItem>
            <SidebarMenuButton>
              {nav_items.user.avatar} {nav_items.user.name}
            </SidebarMenuButton>
          </SidebarMenuItem>
        </SidebarMenu>
      </SidebarFooter>
      <SidebarRail />
    </Sidebar>
  );
}
