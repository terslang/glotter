FROM alpine:3.22 as build
RUN apk update && \
    apk add --no-cache \
        build-base \
        cmake \
        nlohmann_json
WORKDIR /app
COPY . .
WORKDIR /app/build
RUN cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make

FROM alpine:3.22
RUN apk update && \
    apk add --no-cache \
        libstdc++ \
        nlohmann_json
WORKDIR /app
COPY --from=build /app/build/glotter .

EXPOSE 9107
ENTRYPOINT ["./glotter"]
