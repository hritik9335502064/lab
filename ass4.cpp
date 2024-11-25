// Here are the implementations for the requested tasks:

// Task (i): Simulate Logical Clock Synchronization
// Lamport's Logical Clock Algorithm
// cpp
// Copy code
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

int lamport_clock = 0;

// Event function to increment the logical clock
void event(const std::string& event_description) {
    lamport_clock++;
    std::cout << event_description << " | Logical Clock: " << lamport_clock << "\n";
}

// Send event
int send_event() {
    lamport_clock++;
    std::cout << "Send Event | Logical Clock: " << lamport_clock << "\n";
    return lamport_clock;
}

// Receive event
void receive_event(int received_clock) {
    lamport_clock = std::max(lamport_clock, received_clock) + 1;
    std::cout << "Receive Event | Logical Clock: " << lamport_clock << "\n";
}

int main() {
    std::cout << "Simulating Lamport's Logical Clock\n";

    event("Process A performs an internal event.");
    int msg_clock = send_event();
    receive_event(msg_clock);

    event("Process B performs an internal event.");
    receive_event(send_event());

    return 0;
}
Vector Clocks
cpp
Copy code
#include <iostream>
#include <vector>
#include <algorithm>

class VectorClock {
private:
    std::vector<int> clock;
    int process_id;

public:
    VectorClock(int num_processes, int id) : clock(num_processes, 0), process_id(id) {}

    void event() {
        clock[process_id]++;
        print_clock("Internal Event");
    }

    std::vector<int> send_event() {
        clock[process_id]++;
        print_clock("Send Event");
        return clock;
    }

    void receive_event(const std::vector<int>& received_clock) {
        for (size_t i = 0; i < clock.size(); i++) {
            clock[i] = std::max(clock[i], received_clock[i]);
        }
        clock[process_id]++;
        print_clock("Receive Event");
    }

    void print_clock(const std::string& event_description) const {
        std::cout << event_description << " | Vector Clock: [";
        for (size_t i = 0; i < clock.size(); i++) {
            std::cout << clock[i] << (i == clock.size() - 1 ? "]\n" : ", ");
        }
    }
};

int main() {
    const int num_processes = 2;
    VectorClock process1(num_processes, 0);
    VectorClock process2(num_processes, 1);

    process1.event();
    std::vector<int> msg_clock = process1.send_event();
    process2.receive_event(msg_clock);

    process2.event();
    msg_clock = process2.send_event();
    process1.receive_event(msg_clock);

    return 0;
}
// Task (ii): Distributed Mutual Exclusion Algorithms
// Lamport's Distributed Mutual Exclusion Algorithm
// cpp
// Copy code
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <mutex>

struct Request {
    int timestamp;
    int process_id;

    bool operator<(const Request& other) const {
        return timestamp < other.timestamp || (timestamp == other.timestamp && process_id < other.process_id);
    }
};

class LamportMutex {
private:
    int clock;
    int process_id;
    std::priority_queue<Request, std::vector<Request>, std::greater<Request>> request_queue;
    bool critical_section;

public:
    LamportMutex(int id) : clock(0), process_id(id), critical_section(false) {}

    void request_critical_section() {
        clock++;
        Request req{clock, process_id};
        request_queue.push(req);
        std::cout << "Process " << process_id << " requested CS at time " << clock << "\n";
    }

    void release_critical_section() {
        if (!critical_section) return;

        critical_section = false;
        request_queue.pop();
        std::cout << "Process " << process_id << " released CS\n";
    }

    void grant_permission(const Request& req) {
        clock = std::max(clock, req.timestamp) + 1;
        request_queue.push(req);
        std::cout << "Process " << process_id << " granted permission to " << req.process_id << "\n";
    }

    void process() {
        if (!critical_section && !request_queue.empty() && request_queue.top().process_id == process_id) {
            critical_section = true;
            std::cout << "Process " << process_id << " entered CS\n";
        }
    }
};

int main() {
    LamportMutex process1(1);
    LamportMutex process2(2);

    process1.request_critical_section();
    process2.grant_permission({1, 1});
    process2.request_critical_section();

    process1.grant_permission({2, 2});
    process1.process();
    process2.process();

    process1.release_critical_section();
    process2.process();

    return 0;
}
// Ricart-Agrawala Algorithm
// cpp
// Copy code
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>

class RicartAgrawala {
private:
    int clock;
    int process_id;
    bool requesting_cs;
    std::vector<bool> responses;

public:
    RicartAgrawala(int id, int num_processes)
        : clock(0), process_id(id), requesting_cs(false), responses(num_processes, false) {}

    void request_critical_section() {
        clock++;
        requesting_cs = true;
        std::cout << "Process " << process_id << " requested CS at time " << clock << "\n";
        std::fill(responses.begin(), responses.end(), false);
    }

    void receive_request(int other_id, int other_clock) {
        clock = std::max(clock, other_clock) + 1;
        std::cout << "Process " << process_id << " received request from Process " << other_id << "\n";

        if (!requesting_cs || other_clock < clock || (other_clock == clock && other_id < process_id)) {
            responses[other_id] = true;
            std::cout << "Process " << process_id << " sent reply to Process " << other_id << "\n";
        }
    }

    bool can_enter_critical_section() {
        return std::all_of(responses.begin(), responses.end(), [](bool response) { return response; });
    }

    void release_critical_section() {
        requesting_cs = false;
        std::cout << "Process " << process_id << " released CS\n";
    }
};

int main() {
    const int num_processes = 2;

    RicartAgrawala process1(1, num_processes);
    RicartAgrawala process2(2, num_processes);

    process1.request_critical_section();
    process2.receive_request(1, 1);

    if (process1.can_enter_critical_section()) {
        std::cout << "Process 1 entered CS\n";
        process1.release_critical_section();
    }

    process2.request_critical_section();
    process1.receive_request(2, 2);

    if (process2.can_enter_critical_section()) {
        std::cout << "Process 2 entered CS\n";
        process2.release_critical_section();
    }

    return 0;
}
// These programs simulate Lamport's logical clock, vector clocks, and distributed mutual exclusion algorithms. You can expand these as needed for more processes or interactions.