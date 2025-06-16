/** @type {import('next').NextConfig} */
const nextConfig = {

    reactStrictMode: true,

    async rewrites() {
        return [
            {
                source: '/api/render/:job_id/result',
                destination: 'http://localhost:8080/api/render/:job_id/result',
            },
            {
                source: '/api/render/:job_id',
                destination: 'http://localhost:8080/api/render/:job_id',
            },
            {
                source: '/api/render',
                destination: 'http://localhost:8080/api/render',
            },
            {
                source: "/healthz",
                destination: "http://localhost:8080/healthz",
            },
        ];
    }
};

export default nextConfig;
