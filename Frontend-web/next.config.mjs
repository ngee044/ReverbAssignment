/** @type {import('next').NextConfig} */
const nextConfig = {

    reactStrictMode: true,

    async rewrites() {
        return [
            {
                source: '/api/render',
                destination: 'http://localhost:8000/api/render',
            },
            {
                source: '/api/render/:job_id',
                destination: 'http://localhost:8000/api/render/:job_id',
            },
            {
                source: '/api/render/:job_id/result',
                destination: 'http://localhost:8000/api/render/:job_id/result',
            },
            {
                source: "/healthz",
                destination: "http://localhost:8080/healthz",
            },
        ];
    }
};

export default nextConfig;
