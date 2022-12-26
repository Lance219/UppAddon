struct VectorConstructNValueD {
    VectorConstructNValueD()
    {
    }
    VectorConstructNValueD(const VectorConstructNValueD& d)
        : d(d.d)
    {
        if (++i == 11)
            throw 5;
    }

    void DoPostRelocationAdjustment() noexcept
    {
    }
    double d;

    static int i;
};

// some class that is not Upp::Moveable but can still
// benefit from adjustment after relocation for lz::vector

class RelocAdjustSimple {
    struct Node {
        RelocAdjustSimple* owner;
        char buff[1024];

        char* UseBuff()
        {
            return buff;
        }
    };

    Node* node;
    char buff[1024];

public:
    RelocAdjustSimple() noexcept
        : node(nullptr)
    {
    }

    RelocAdjustSimple(const RelocAdjustSimple& ras)
    {
        memcpy(this, &ras, sizeof(ras));
        node = nullptr;
        if (ras.node != nullptr) {
            node = new Node;
            memcpy(node, ras.node, sizeof(*node));
            node->owner = this;
        }
    }
    RelocAdjustSimple(RelocAdjustSimple&& ras) noexcept
    {
        memcpy(this, &ras, sizeof(ras));
        if (node)
            node->owner = this;
        ras.node = nullptr;
    }
    ~RelocAdjustSimple() noexcept
    {
        delete node; /* LOG("RelocAdjustSimple Object destructed");*/
    }

    void AddNode()
    {
        if (!node)
            node = new Node { this };
    }

    void DoPostRelocationAdjustment() noexcept
    {
        if (node)
            node->owner = this;
    }

    bool IsConsistent() const
    {
        return node->owner == this;
    }

    char* UseBuff()
    {
        return buff;
    }
};