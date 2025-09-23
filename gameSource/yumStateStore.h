#ifndef __YUM_STATE_STORE_H__
#define __YUM_STATE_STORE_H__

#include <string>
#include <map>
#include <vector>
#include <deque>

// Atomically-updated disk storage for small amounts of data. This is intended
// for relatively slow-moving (human speed) data that the server doesn't re-send
// on reconnect or that only exists within the mod.
//
// Items are stored as field names with an arbitrary sequence of values. New
// field names can be added at any time, but be careful to keep your reading
// and writing code backward-compatible for existing field names. This means
// having reasonable defaults for get() and push()ing new fields only after
// existing ones that older readers will understand.

class yumStateStore {
    public:
        yumStateStore(const char *filename);
        ~yumStateStore();


        // To read the state:
        //   yumStateStore store("state.dat");
        //   store.read();
        //   std::string key;
        //   while (store.nextField(key)) {
        //       if (key == "some_known_key") {
        //           std::string value;
        //           while (store.get(value)) {
        //               // do something with value
        //           }
        //       } ...
        //   }
        // Do not modify the store while reading.
        void read();
        bool isReading() const { return !mKeys.empty(); }
        // Populates the next field name. Returns false when there are no more
        // fields.
        bool nextField(std::string &key);
        // Can be called repeatedly for multiple values within a key. Returns
        // false when there are no more values at the current key.
        bool get(std::string &value);
        bool get(int &value);
        bool get(double &value);
        // Get the first value for a specific key.
        bool get(const char *key, std::string &value);
        bool get(const char *key, int &value);
        bool get(const char *key, double &value);

        // Keys cannot contain =. Avoid using keys that aren't in [A-Za-z0-9_-.]
        // in case encoding changes in the future.
        // Values are arbitrary.
        void clear(const char *key);
        void clearAll();
        void push(const char *key, const char *value);
        void push(const char *key, int value);
        void push(const char *key, double value);

        // Calling hasChanged() isn't mandatory, but it's useful for batching
        // writes to some interval.
        inline bool hasChanged() const { return mChanged; };
        void write();

    private:
        std::string mFilename;
        std::map<std::string, std::vector<std::string>> mState;
        bool mChanged;

        // state for nextField() / get()
        std::deque<std::string> mKeys;
        std::vector<std::string>::iterator mValuesIterator;
        std::vector<std::string>::iterator mValuesEnd;
};

#endif // __YUM_STATE_STORE_H__