# This script runs the development container for RANDA

echo "Creating user data..."
docker volume create user_data_randa

echo "Running dev image..."
docker run -d -it --name randa_dev -v user_data_randa:/root/user_data_randa randa_dev:latest

docker exec randa_dev mkdir -p /root/user_data_randa/.ssh/
docker exec randa_dev mkdir -p /root/user_data_randa/.vscode-server/
