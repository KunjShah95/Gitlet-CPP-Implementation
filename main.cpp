#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <stdexcept>

unsigned long simpleHash(const std::string &str)
{
    unsigned long hash = 5831;
    for (char c : str)
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    hash += str.length() * 7919;
    return hash;
}

std::string hashToString(unsigned long hash)
{
    std::ostringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

template <typename T>
class SimpleVec
{
private:
    T *data;
    size_t count;
    size_t capacity;

    void resize(size_t newCapacity)
    {
        T *newData = new T[newCapacity];
        for (size_t i = 0; i < count; ++i)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    SimpleVec() : data(nullptr), count(0), capacity(0) {}

    SimpleVec(const SimpleVec &other) : data(nullptr), count(0), capacity(0)
    {
        if (other.capacity > 0)
        {
            data = new T[other.capacity];
            capacity = other.capacity;
            count = other.count;
            for (size_t i = 0; i < count; ++i)
            {
                data[i] = other.data[i];
            }
        }
    }

    SimpleVec &operator=(const SimpleVec &other)
    {
        if (this == &other)
        {
            return *this;
        }
        delete[] data;

        data = nullptr;
        count = 0;
        capacity = 0;

        if (other.capacity > 0)
        {
            data = new T[other.capacity];
            capacity = other.capacity;
            count = other.count;
            for (size_t i = 0; i < count; ++i)
            {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    ~SimpleVec()
    {
        delete[] data;
    }
    void push_back(const T &value)
    {
        if (count == capacity)
        {
            resize(capacity == 0 ? 8 : capacity * 2);
        }
        data[count++] = value;
    }

    T &get(size_t index)
    {
        if (index >= count)
        {
            throw std::out_of_range("SimpleVec Index out of range");
        }
        return data[index];
    }

    const T &get(size_t index) const
    {
        if (index >= count)
        {
            throw std::out_of_range("SimpleVec Index out of range (const)");
        }
        return data[index];
    }

    T &operator[](size_t index)
    {
        return get(index);
    }

    const T &operator[](size_t index) const
    {
        return get(index);
    }

    size_t size() const
    {
        return count;
    }

    bool empty() const
    {
        return count == 0;
    }

    void clear()
    {
        count = 0;
    }
};

void bubbleSort(SimpleVec<std::string> &vec)
{
    size_t n = vec.size();
    if (n < 2)
    {
        return;
    }
    bool swapped;
    do
    {
        swapped = false;
        for (size_t i = 0; i < n - 1; i++)
        {
            if (vec[i] > vec[i + 1])
            {
                std::string temp = vec[i];
                vec[i] = vec[i + 1];
                vec[i + 1] = temp;
                swapped = true;
            }
        }
        n--;
    } while (swapped);
}

template <typename K, typename V>
class SimpleMap
{
private:
    struct Node
    {
        K key;
        V value;
        Node *next;

        Node(const K &k, const V &v) : key(k), value(v), next(nullptr) {}
    };

    Node **buckets;
    size_t numBuckets;
    size_t count;
    static const size_t INITIAL_BUCKETS = 16;
    static constexpr double MAX_LOAD_FACTOR = 0.75;

    size_t hashFn(const K &key) const
    {
        if constexpr (std::is_same<K, std::string>::value)
        {
            return simpleHash(key) % numBuckets;
        }
        else
        {
            static_assert(std::is_same<K, std::string>::value, "SimpleMap only supports string keys in this example");
            return 0;
        }
    }
    void resize()
    {
        size_t oldNumBuckets = numBuckets;
        Node **oldBuckets = buckets;

        numBuckets *= 2;
        buckets = new Node *[numBuckets];
        for (size_t i = 0; i < numBuckets; ++i)
            buckets[i] = nullptr;
        count = 0;

        for (size_t i = 0; i < oldNumBuckets; ++i)
        {
            Node *current = oldBuckets[i];
            while (current != nullptr)
            {
                insert(current->key, current->value);
                Node *toDelete = current;
                current = current->next;
                delete toDelete;
            }
        }
        delete[] oldBuckets;
    }

    void copyFrom(const SimpleMap &other)
    {
        if (other.numBuckets == 0)
        {
            return;
        }

        numBuckets = other.numBuckets;
        buckets = new Node *[numBuckets];
        for (size_t i = 0; i < numBuckets; ++i)
            buckets[i] = nullptr;
        count = 0;

        for (size_t i = 0; i < other.numBuckets; ++i)
        {
            Node *otherCurrent = other.buckets[i];
            while (otherCurrent != nullptr)
            {
                insert(otherCurrent->key, otherCurrent->value);
                otherCurrent = otherCurrent->next;
            }
        }
    }

public:
    SimpleMap() : buckets(nullptr), numBuckets(0), count(0)
    {
        numBuckets = INITIAL_BUCKETS;
        buckets = new Node *[numBuckets];
        for (size_t i = 0; i < numBuckets; ++i)
            buckets[i] = nullptr;
    }

    SimpleMap(const SimpleMap &other) : buckets(nullptr), numBuckets(0), count(0)
    {
        copyFrom(other);
    }

    SimpleMap &operator=(const SimpleMap &other)
    {
        if (this == &other)
        {
            return *this;
        }
        clear();
        delete[] buckets;

        buckets = nullptr;
        numBuckets = 0;
        count = 0;

        copyFrom(other);
        return *this;
    }

    ~SimpleMap()
    {
        clear();
        delete[] buckets;
    }

    void insert(const K &key, const V &value)
    {
        if ((double)(count + 1) / numBuckets > MAX_LOAD_FACTOR)
        {
            resize();
        }

        size_t bucketIndex = hashFn(key);
        Node *head = buckets[bucketIndex];
        Node *current = head;
        while (current != nullptr)
        {
            if (current->key == key)
            {
                current->value = value;
                return;
            }
            current = current->next;
        }
        Node *newNode = new Node(key, value);
        newNode->next = head;
        buckets[bucketIndex] = newNode;
        count++;
    }

    V *find(const K &key)
    {
        size_t bucketIndex = hashFn(key);
        Node *current = buckets[bucketIndex];
        while (current != nullptr)
        {
            if (current->key == key)
            {
                return &(current->value);
            }
            current = current->next;
        }
        return nullptr;
    }
    const V *find(const K &key) const
    {
        size_t bucketIndex = hashFn(key);
        Node *current = buckets[bucketIndex];
        while (current != nullptr)
        {
            if (current->key == key)
            {
                return &(current->value);
            }
            current = current->next;
        }
        return nullptr;
    }

    bool contains(const K &key) const
    {
        return find(key) != nullptr;
    }

    bool remove(const K &key)
    {
        size_t bucketIndex = hashFn(key);
        Node *current = buckets[bucketIndex];
        Node *prev = nullptr;

        while (current != nullptr)
        {
            if (current->key == key)
            {
                if (prev == nullptr)
                {
                    buckets[bucketIndex] = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                delete current;
                count--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    void clear()
    {
        for (size_t i = 0; i < numBuckets; i++)
        {
            Node *current = buckets[i];
            while (current != nullptr)
            {
                Node *toDelete = current;
                current = current->next;
                delete toDelete;
            }
            buckets[i] = nullptr;
        }
        count = 0;
    }

    size_t size() const
    {
        return count;
    }

    bool empty() const
    {
        return count == 0;
    }

    SimpleVec<K> getKeys() const
    {
        SimpleVec<K> keys;
        for (size_t i = 0; i < numBuckets; ++i)
        {
            Node *current = buckets[i];
            while (current != nullptr)
            {
                keys.push_back(current->key);
                current = current->next;
            }
        }
        return keys;
    };
};

struct Commit
{
    std::string id;
    std::string message;
    long timestamp;
    std::string parentId;
    SimpleMap<std::string, std::string> trackedFiles;
    Commit() : timestamp(0) {}
};

class Gitlet
{
private:
    bool initialized;
    SimpleMap<std::string, std::string> stagingArea; // filename -> contentHash
    SimpleMap<std::string, std::string> objectStore; // contentHash -> content
    SimpleMap<std::string, Commit> commits;          // commitId -> Commit object
    std::string headCommitId;

    std::string generateCommitId(const Commit &commit)
    {
        std::stringstream dataStream;
        dataStream << commit.message << commit.timestamp << commit.parentId;

        // Get keys, sort them, then access values for hashing
        SimpleVec<std::string> sortedFilenames = commit.trackedFiles.getKeys();
        bubbleSort(sortedFilenames); // Use our simple sort

        for (size_t i = 0; i < sortedFilenames.size(); ++i)
        {
            const std::string &fname = sortedFilenames[i];
            const std::string *contentHashPtr = commit.trackedFiles.find(fname); // find returns pointer
            if (contentHashPtr)
            { // Should always be found if key came from getKeys
                dataStream << fname << *contentHashPtr;
            }
            else
            {
                // This indicates an internal error
                std::cerr << "Error: Key found in getKeys but not in find during hash generation for " << fname << std::endl;
            }
        }

        return hashToString(simpleHash(dataStream.str()));
    }

    Commit *getHeadCommit()
    {
        if (headCommitId.empty())
            return nullptr;
        return commits.find(headCommitId); // find returns pointer, null if not found
    }

    Commit *getCommit(const std::string &commitId)
    {
        return commits.find(commitId); // find returns pointer, null if not found
    }

public:
    Gitlet() : initialized(false) {}

    // --- Core Commands ---

    void init()
    {
        if (initialized)
        {
            std::cout << "Repository already initialized." << std::endl;
            return;
        }
        commits.clear(); // Ensure clean state
        stagingArea.clear();
        objectStore.clear();

        Commit initialCommit;
        initialCommit.message = "initial commit";
        initialCommit.timestamp = std::time(nullptr);
        initialCommit.parentId = "";
        // trackedFiles is empty by default constructor of SimpleMap

        initialCommit.id = generateCommitId(initialCommit);

        // Insert the Commit object (requires Commit copyability)
        commits.insert(initialCommit.id, initialCommit);

        headCommitId = initialCommit.id;
        initialized = true;

        std::cout << "Initialized empty Gitlet repository." << std::endl;
        std::cout << "Initial commit ID: " << headCommitId << std::endl;
    }

    void add(const std::string &filename, const std::string &content)
    {
        if (!initialized)
        {
            std::cout << "Error: Repository not initialized. Run 'init' first." << std::endl;
            return;
        }

        std::string contentHash = hashToString(simpleHash(content));

        // Store blob if new
        if (!objectStore.contains(contentHash))
        {
            objectStore.insert(contentHash, content);
        }

        // Check if identical to version in HEAD commit
        Commit *head = getHeadCommit();
        bool identicalToHead = false;
        if (head)
        {
            const std::string *headContentHashPtr = head->trackedFiles.find(filename);
            if (headContentHashPtr && *headContentHashPtr == contentHash)
            {
                identicalToHead = true;
            }
        }

        // Check staging area
        const std::string *stagedContentHashPtr = stagingArea.find(filename);

        // If identical to head, remove from staging
        if (identicalToHead)
        {
            if (stagedContentHashPtr)
            { // Only remove if it was actually staged
                stagingArea.remove(filename);
                // std::cout << "Debug: Unstaged '" << filename << "' as it matches HEAD." << std::endl;
            }
        }
        else
        {
            // Stage the file: filename -> contentHash
            // Only print message if it's a new staging or different content
            if (!stagedContentHashPtr || *stagedContentHashPtr != contentHash)
            {
                stagingArea.insert(filename, contentHash);
                std::cout << "Staged '" << filename << "' for commit." << std::endl;
            }
            else
            {
                // std::cout << "Debug: File '" << filename << "' already staged with this content." << std::endl;
            }
        }
    }

    void commit(const std::string &message)
    {
        if (!initialized)
        {
            std::cout << "Error: Repository not initialized. Run 'init' first." << std::endl;
            return;
        }
        if (stagingArea.empty())
        {
            std::cout << "Nothing to commit, staging area is empty." << std::endl;
            return;
        }

        Commit *parentCommit = getHeadCommit();
        if (!parentCommit)
        {
            std::cerr << "Critical Error: HEAD commit not found!" << std::endl;
            return;
        }

        Commit newCommit;
        newCommit.message = message;
        newCommit.timestamp = std::time(nullptr);
        newCommit.parentId = headCommitId;

        // Inherit tracked files from parent (uses SimpleMap assignment operator)
        newCommit.trackedFiles = parentCommit->trackedFiles;

        // Apply staged changes
        SimpleVec<std::string> stagedKeys = stagingArea.getKeys();
        for (size_t i = 0; i < stagedKeys.size(); ++i)
        {
            const std::string &filename = stagedKeys[i];
            std::string *contentHashPtr = stagingArea.find(filename); // Find pointer
            if (contentHashPtr)
            {
                newCommit.trackedFiles.insert(filename, *contentHashPtr); // Insert/update in commit
            }
            else
            {
                std::cerr << "Error: Staged key '" << filename << "' not found during commit." << std::endl;
            }
        }

        newCommit.id = generateCommitId(newCommit);
        commits.insert(newCommit.id, newCommit); // Insert copy
        headCommitId = newCommit.id;
        stagingArea.clear();

        std::cout << "Committed changes with ID: " << newCommit.id << std::endl;
    }

    void log()
    {
        if (!initialized)
        {
            std::cout << "Error: Repository not initialized. Run 'init' first." << std::endl;
            return;
        }

        std::cout << "--- Commit History ---" << std::endl;
        std::string currentCommitId = headCommitId;

        while (!currentCommitId.empty())
        {
            Commit *currentCommit = getCommit(currentCommitId); // Returns pointer
            if (!currentCommit)
            {
                std::cerr << "Error: Commit data missing for ID: " << currentCommitId << std::endl;
                break;
            }

            char timeBuf[80];
            std::time_t commitTime = currentCommit->timestamp;
            std::strftime(timeBuf, sizeof(timeBuf), "%a %b %d %H:%M:%S %Y %z", std::localtime(&commitTime));

            std::cout << "Commit: " << currentCommit->id << std::endl;
            std::cout << "Date:   " << timeBuf << std::endl;
            std::cout << "Message:" << currentCommit->message << std::endl;
            std::cout << "Files:   ";

            SimpleVec<std::string> trackedKeys = currentCommit->trackedFiles.getKeys();
            if (trackedKeys.empty())
            {
                std::cout << "(none)" << std::endl;
            }
            else
            {
                bubbleSort(trackedKeys); // Sort for consistent output
                bool first = true;
                for (size_t i = 0; i < trackedKeys.size(); ++i)
                {
                    const std::string &fname = trackedKeys[i];
                    const std::string *hashPtr = currentCommit->trackedFiles.find(fname);
                    if (!first)
                        std::cout << ", ";
                    std::cout << fname;
                    if (hashPtr)
                        std::cout << " (" << hashPtr->substr(0, 6) << "...)";
                    first = false;
                }
                std::cout << std::endl;
            }

            std::cout << "--------------------" << std::endl;
            currentCommitId = currentCommit->parentId; // Move to parent
        }
    }

    void checkout(const std::string &commitIdOrPrefix)
    {
        if (!initialized)
        {
            std::cout << "Error: Repository not initialized. Run 'init' first." << std::endl;
            return;
        }

        std::string targetCommitId = "";

        // Check for exact match first
        if (commits.contains(commitIdOrPrefix))
        {
            targetCommitId = commitIdOrPrefix;
        }
        else
        {
            // Try prefix matching
            SimpleVec<std::string> allCommitIds = commits.getKeys();
            int matchCount = 0;
            for (size_t i = 0; i < allCommitIds.size(); ++i)
            {
                const std::string &id = allCommitIds[i];
                // Check if id starts with commitIdOrPrefix
                if (id.rfind(commitIdOrPrefix, 0) == 0)
                {
                    matchCount++;
                    targetCommitId = id; // Store potential match
                }
            }

            if (matchCount == 0)
            {
                std::cout << "Error: Commit with ID or prefix '" << commitIdOrPrefix << "' not found." << std::endl;
                return;
            }
            else if (matchCount > 1)
            {
                std::cout << "Error: Ambiguous commit ID prefix '" << commitIdOrPrefix << "'." << std::endl;
                return;
            }
            // Exactly one prefix match found, targetCommitId holds the full ID
            std::cout << "Checking out full commit ID: " << targetCommitId << std::endl;
        }

        // Check if the target commit actually exists (should unless internal error)
        if (!commits.contains(targetCommitId))
        {
            std::cerr << "Critical Error: Target commit ID '" << targetCommitId << "' resolved but not found in map!" << std::endl;
            return;
        }

        headCommitId = targetCommitId;
        std::cout << "HEAD is now at commit: " << headCommitId.substr(0, 7) << std::endl;

        if (!stagingArea.empty())
        {
            std::cout << "Warning: Staging area cleared due to checkout." << std::endl;
            stagingArea.clear();
        }
    }

    void printCurrentFileState()
    {
        if (!initialized)
        {
            std::cout << "Error: Repository not initialized." << std::endl;
            return;
        }
        std::cout << "\n--- Files at HEAD (" << headCommitId.substr(0, 7) << ") ---" << std::endl;
        Commit *head = getHeadCommit();
        if (!head)
        {
            std::cerr << "Error: Cannot get HEAD commit." << std::endl;
            return;
        }

        SimpleVec<std::string> trackedKeys = head->trackedFiles.getKeys();
        if (trackedKeys.empty())
        {
            std::cout << "(No files tracked in this commit)" << std::endl;
        }
        else
        {
            bubbleSort(trackedKeys); // Sort for consistent output
            for (size_t i = 0; i < trackedKeys.size(); ++i)
            {
                const std::string &filename = trackedKeys[i];
                const std::string *contentHashPtr = head->trackedFiles.find(filename);
                if (contentHashPtr)
                {
                    const std::string *contentPtr = objectStore.find(*contentHashPtr);
                    if (contentPtr)
                    {
                        std::cout << "'" << filename << "' : \"" << *contentPtr << "\"" << std::endl;
                    }
                    else
                    {
                        std::cout << "'" << filename << "' : (Error: Content blob " << *contentHashPtr << " not found!)" << std::endl;
                    }
                }
                else
                {
                    std::cout << "'" << filename << "' : (Error: Key inconsistency!)" << std::endl;
                }
            }
        }
        std::cout << "--------------------------" << std::endl;

        SimpleVec<std::string> stagedKeys = stagingArea.getKeys();
        if (!stagedKeys.empty())
        {
            std::cout << "\n--- Staging Area ---" << std::endl;
            bubbleSort(stagedKeys); // Sort for consistent output
            for (size_t i = 0; i < stagedKeys.size(); ++i)
            {
                const std::string &filename = stagedKeys[i];
                const std::string *hashPtr = stagingArea.find(filename);
                std::cout << "Staged: '" << filename << "' (Content Hash: ";
                if (hashPtr)
                    std::cout << hashPtr->substr(0, 6);
                else
                    std::cout << "???";
                std::cout << "...)" << std::endl;
            }
            std::cout << "--------------------" << std::endl;
        }
        else
        {
            std::cout << "\n(Staging area is empty)\n" << std::endl;
        }
    }
};

// --- Main Function (Example Usage - Should work as before) ---
int main()
{
    Gitlet repo;

    std::cout << ">>> repo.init();\n";
    repo.init();                                 // Need to capture the ID printed here
    std::string initialCommitId = "PLACEHOLDER"; // *** MANUALLY UPDATE THIS AFTER FIRST RUN ***
    repo.printCurrentFileState();

    std::cout << "\n>>> repo.add(\"file1.txt\", \"Hello\");\n";
    repo.add("file1.txt", "Hello");
    repo.printCurrentFileState();

    std::cout << "\n>>> repo.commit(\"Add file1.txt\");\n";
    repo.commit("Add file1.txt");
    repo.printCurrentFileState();

    std::cout << "\n>>> repo.add(\"file1.txt\", \"Hello World!\");\n";
    repo.add("file1.txt", "Hello World!");
    std::cout << "\n>>> repo.add(\"file2.txt\", \"Another file.\");\n";
    repo.add("file2.txt", "Another file.");
    repo.printCurrentFileState();

    std::cout << "\n>>> repo.commit(\"Update file1, add file2\");\n";
    repo.commit("Update file1, add file2");
    repo.printCurrentFileState();

    std::cout << "\n>>> repo.add(\"file1.txt\", \"Hello World!\"); // Add identical content\n";
    repo.add("file1.txt", "Hello World!");
    repo.printCurrentFileState();

    std::cout << "\n>>> repo.log();\n";
    repo.log();

    std::cout << "\n>>> repo.checkout(...); // Using prefix of initial commit\n";
    std::cout << "!! Important: Update 'initialCommitId' variable above with the actual hash from the 'init' output, then uncomment checkout below !!" << std::endl;
    // if (initialCommitId != "PLACEHOLDER") {
    //     repo.checkout(initialCommitId.substr(0, 6)); // Use a prefix
    //     repo.printCurrentFileState();
    // } else {
    //    std::cout << "Skipping checkout test as initial commit ID was not updated." << std::endl;
    // }

    std::cout << "\n>>> repo.log(); // Log from the checked-out state (if checkout ran)\n";
    repo.log();

    return 0;
}