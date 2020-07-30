#ifndef MINESWEEPERQ_H
#define MINESWEEPERQ_H

#include <QAbstractTableModel>
#include <qqml.h>
#include <vector>
#include <cstdlib>

class MinesweeperQ : public QAbstractTableModel {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int rows READ rowCount WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columnCount WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(int numBombs READ numBombs WRITE setNumBombs NOTIFY numBombsChanged)
    Q_PROPERTY(int numLeft READ numLeft NOTIFY numLeftChanged)
    Q_PROPERTY(int numFlags READ numFlags NOTIFY numFlagsChanged)

private:
    struct block {
        int adjacent;
        bool revealed;
        bool flagged;
    };
    std::vector<std::vector<block> > matrix;
    int num_bombs;
    int num_left;
    int num_flags;

    inline int getRandom(int zero_to);
    void expandBlocks(const int j, const int k, const int max_rows, int max_columns);
    void revealAll();
    void makeMinesweep(int rowSeed, int columnSeed);

public:
    enum DataTypes {
        NumType = Qt::UserRole,
        FlaggedType,
        RevealedType
    };

    // Constructors
    MinesweeperQ(QObject* parent = nullptr) :
    QAbstractTableModel(parent), matrix(1), num_bombs(0), num_left(0), num_flags(0) {
        std::srand(clock());
        matrix[0].push_back({0, false, false});
    }

    // Getter Methods
    inline int getRows() const { return matrix.size(); }
    inline int getColumns() const { return matrix[0].size(); }
    inline int numBombs() const { return num_bombs; }
    inline int numLeft() const { return num_left; }
    inline int numFlags() const { return num_flags; }

    // Setter Methods
    void setRows(int count);
    void setColumns(int count);
    inline void setNumBombs(int a) { num_bombs = a; numBombsChanged(); }

// Inherited Methods
    // Read Methods
    auto rowCount(const QModelIndex& parent = QModelIndex()) const
        -> int override;
    auto columnCount(const QModelIndex& parent = QModelIndex()) const
        -> int override;
    auto data(const QModelIndex& index, int role = Qt::DisplayRole) const
        -> QVariant override;
    auto headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
        -> QVariant override;
    auto roleNames() const
        -> QHash<int, QByteArray> override;

    // Write Methods
    auto setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole)
        -> bool override;
    auto flags(const QModelIndex& index) const
        -> Qt::ItemFlags override;

    // Resize Methods
    auto insertRows(int before_row, int count, const QModelIndex& parent = QModelIndex())
        -> bool override;
    auto removeRows(int from_row, int count, const QModelIndex& parent = QModelIndex())
        -> bool override;
    auto insertColumns(int before_column, int count, const QModelIndex& parent = QModelIndex())
        -> bool override;
    auto removeColumns(int from_column, int count, const QModelIndex& parent = QModelIndex())
        -> bool override;

    ~MinesweeperQ() {};

public slots:
    void reset();

signals:
    void rowsChanged();
    void columnsChanged();
    void numBombsChanged();
    void numLeftChanged();
    void numFlagsChanged();
};

#endif // MINESWEEPERQ_H
