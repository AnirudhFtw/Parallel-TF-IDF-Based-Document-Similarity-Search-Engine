
#include <bits/stdc++.h>
#include <filesystem>
#include <omp.h>
using namespace std;
namespace fs = std::filesystem;

// -------- Tokenizer --------
vector<string> tokenize(const string &text) {
    vector<string> tokens;
    string cur;
    for (char c : text) {
        if (isalpha((unsigned char)c)) cur.push_back(tolower((unsigned char)c));
        else {
            if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

// -------- Stopword List --------
const unordered_set<string> STOPWORDS = {
    "the","is","and","a","an","of","to","in","that","it","on","for","as","with","this","by","be","are"
};

// -------- File Reader --------
string read_file(const fs::path &p) {
    ifstream ifs(p);
    if (!ifs) return {};
    stringstream ss; ss << ifs.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    cout << "Name 1: Anirudh Sandeep Gupta              Roll No:2023BCS0209\nName 2: Aditya Dubey                              Roll No:2023BCD0062\n";
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <docs_folder>\n";
        return 1;
    }
    string folder = argv[1];

    // Step 1: Collect document file paths
    vector<fs::path> files;
    for (auto &entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file()) files.push_back(entry.path());
    }
    sort(files.begin(), files.end());
    int N = files.size();
    if (N == 0) { cerr << "No files found in folder.\n"; return 1; }

    vector<unordered_map<string,int>> doc_term_counts(N);
    vector<int> doc_total_terms(N,0);

    double t0 = omp_get_wtime();

    // -------- Phase 1: Tokenization (parallel) --------
    #pragma omp parallel for schedule(dynamic)
    for (int d=0; d<N; ++d) {
        string txt = read_file(files[d]);
        auto toks = tokenize(txt);
        unordered_set<string> seen;
        for (auto &tk : toks) {
            if (STOPWORDS.count(tk)) continue;
            ++doc_term_counts[d][tk];
            ++doc_total_terms[d];
        }
    }

    // -------- Phase 2: Compute document frequency (parallel reduction) --------
    unordered_map<string,int> doc_freq;
    #pragma omp parallel
    {
        unordered_map<string,int> local_df;
        #pragma omp for nowait
        for (int d=0; d<N; ++d) {
            unordered_set<string> seen;
            for (auto &kv : doc_term_counts[d]) seen.insert(kv.first);
            for (auto &t : seen) local_df[t]++;
        }
        #pragma omp critical
        {
            for (auto &kv : local_df) doc_freq[kv.first]+=kv.second;
        }
    }

    // -------- Phase 3: Vocabulary indexing --------
    unordered_map<string,int> vocab_index;
    int V=0;
    for (auto &p : doc_freq) vocab_index[p.first]=V++;

    // -------- Phase 4: TF-IDF computation (parallel) --------
    vector<unordered_map<int,double>> doc_vectors(N);
    #pragma omp parallel for
    for (int d=0; d<N; ++d) {
        double norm=0;
        for (auto &kv : doc_term_counts[d]) {
            const string &term = kv.first;
            int cnt = kv.second;
            double tf = double(cnt) / double(max(1,doc_total_terms[d]));
            double idf = log(double(N)/double(doc_freq[term]) + 1.0);
            double tfidf = tf*idf;
            int idx = vocab_index[term];
            doc_vectors[d][idx]=tfidf;
            norm += tfidf*tfidf;
        }
        norm = sqrt(norm);
        if (norm>0) {
            for (auto &kv : doc_vectors[d]) kv.second /= norm;
        }
    }

    double t1 = omp_get_wtime();
    cout << "Indexed " << N << " documents, vocab size=" << V 
         << ", build time=" << (t1-t0)*1000 << " ms\n";

    // -------- Query loop (serial, interactive) --------
    while (true) {
        cout << "\nDo you want to continue searching? (yes/no): ";
        string choice; 
        if (!(cin >> choice)) break;
        if (choice=="no" || choice=="No" || choice=="NO") {
            cout << "Exiting search engine. Bye!\n";
            break;
        }
        if (choice!="yes" && choice!="Yes" && choice!="YES") {
            cout << "Invalid input, please type yes or no.\n";
            continue;
        }

        cin.ignore();
        cout << "Enter your query: ";
        string qline; getline(cin,qline);

        auto qtokens = tokenize(qline);
        unordered_map<int,double> qvec;
        int qtot=0;
        unordered_set<string> seenq;
        for (auto &tk : qtokens) {
            if (STOPWORDS.count(tk)) continue;
            seenq.insert(tk);
            ++qtot;
        }
        double qnorm=0;
        for (auto &tk : seenq) {
            auto itv = vocab_index.find(tk);
            if (itv==vocab_index.end()) continue;
            int idx = itv->second;
            int cnt=0;
            for (auto &w:qtokens) if (w==tk) ++cnt;
            double tf = double(cnt)/double(max(1,qtot));
            double idf = log(double(N)/double(doc_freq[tk]) + 1.0);
            double tfidf = tf*idf;
            qvec[idx]=tfidf;
            qnorm += tfidf*tfidf;
        }
        qnorm=sqrt(qnorm);
        if (qnorm>0) for (auto &kv:qvec) kv.second/=qnorm;

        vector<pair<int,double>> scores;
        for (int d=0; d<N; ++d) {
            double s=0;
            for (auto &kv:qvec) {
                auto it=doc_vectors[d].find(kv.first);
                if (it!=doc_vectors[d].end()) s+=kv.second*it->second;
            }
            if (s>0) scores.emplace_back(d,s);
        }
        sort(scores.begin(),scores.end(),[](auto&a,auto&b){return a.second>b.second;});

        cout << "Top results:\n";
        for (int i=0;i<min((int)scores.size(),10);++i) {
            cout << i+1 << ") " << files[scores[i].first].filename().string()
                 << " (score=" << fixed << setprecision(4) << scores[i].second << ")\n";
        }
        if (scores.empty()) cout << "No relevant documents found.\n";
    }

    return 0;
}
