#!/usr/bin/env bash

# Simple MCP client to exercise TheIDE MCP server over TCP.
# Usage:
#   ./mcp_client.sh <port> [host]
# Defaults: port from env MCP_PORT or 7326, host 127.0.0.1

set -euo pipefail

PORT="${1:-${MCP_PORT:-7326}}"
HOST="${2:-127.0.0.1}"

send() {
  local json="$1"
  # Use bash TCP redirection to send a single line and print response
  exec 3<>"/dev/tcp/${HOST}/${PORT}" || {
    echo "Cannot connect to ${HOST}:${PORT}" >&2
    exit 1
  }
  printf '%s\n' "${json}" >&3
  # Read a single response line
  IFS= read -r line <&3 || true
  exec 3>&-
  echo "${line}"
}

req() {
  local id="$1"; shift
  local method="$1"; shift
  local params="$1"; shift || true
  if [[ -z "${params}" ]]; then params='{}'; fi
  send "{\"jsonrpc\":\"2.0\",\"id\":\"${id}\",\"method\":\"${method}\",\"params\":${params}}"
}

# Prefer discovery via mcp.capabilities; fallback to grep
CAP=$(req cap mcp.capabilities '{}') || true
METHODS=( )
if [[ -n "$CAP" && "$CAP" == *"methods"* ]]; then
  # crude JSON extraction for methods array
  METHODS_STR=$(echo "$CAP" | sed -n 's/.*"methods"[[:space:]]*:[[:space:]]*\[\(.*\)\].*/\1/p')
  IFS=',' read -r -a METHODS <<< "${METHODS_STR//\"/}"
fi
if [[ ${#METHODS[@]} -eq 0 ]]; then
  SRC_DIR="$(cd "$(dirname "$0")" && pwd)"
  SERVER_CPP="${SRC_DIR}/Server.cpp"
  if [[ -r "${SERVER_CPP}" ]]; then
    while IFS= read -r m; do METHODS+=("${m}"); done < <(
      grep -oE 'req.method\s*==\s*"[a-zA-Z0-9_.]+' "${SERVER_CPP}" | sed -E 's/.*"//' | sort -u
    )
  fi
fi
if [[ ${#METHODS[@]} -eq 0 ]]; then METHODS=( "mcp.ping" "workspace.info" ); fi

id=1
for method in "${METHODS[@]}"; do
  echo "== ${method} =="
  req "$id" "$method" '{}'
  id=$((id+1))
done
