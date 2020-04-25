ps -ef | grep "python" | awk '{print $2}' | xargs sudo kill
