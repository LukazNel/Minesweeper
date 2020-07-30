#include "../include/minesweeperQ.h"

int MinesweeperQ::getRandom(int zero_to) {
    return rand() % zero_to;
}

void MinesweeperQ::expandBlocks(const int j, const int k, const int max_rows, int max_columns) {
    // Top row
    if (j > 0) {
        // Left
        if (k > 0 && matrix[j-1][k-1].adjacent != -1 && matrix[j-1][k-1].revealed != true) {
            matrix[j-1][k-1].revealed = true;
            num_left--;
            if (matrix[j-1][k-1].adjacent == 0) {
                expandBlocks(j-1, k-1, max_rows, max_columns);
            }
        }

        // Center
        if (matrix[j-1][k].adjacent != -1 && matrix[j-1][k].revealed != true) {
            matrix[j-1][k].revealed = true;
            num_left--;
            if (matrix[j-1][k].adjacent == 0) {
                expandBlocks(j-1, k, max_rows, max_columns);
            }
        }

        // Right
        if (k < max_columns && matrix[j-1][k+1].adjacent != -1 && matrix[j-1][k+1].revealed != true) {
            matrix[j-1][k+1].revealed = true;
            num_left--;
            if (matrix[j-1][k+1].adjacent == 0) {
                expandBlocks(j-1, k+1, max_rows, max_columns);
            }
        }
    }

    // Middle Row
    if (k > 0 && matrix[j][k-1].adjacent != -1 && matrix[j][k-1].revealed != true) {
        matrix[j][k-1].revealed = true;
        num_left--;
        if (matrix[j][k-1].adjacent == 0) {
            expandBlocks(j, k-1, max_rows, max_columns);
        }
    }

    if (k < max_columns && matrix[j][k+1].adjacent != -1 && matrix[j][k+1].revealed != true) {
        matrix[j][k+1].revealed = true;
        num_left--;
        if (matrix[j][k+1].adjacent == 0) {
            expandBlocks(j, k+1, max_rows, max_columns);
        }
    }

    // Bottom Row
    if (j < max_rows) {
        // Left
        if (k > 0 && matrix[j+1][k-1].adjacent != -1 && matrix[j+1][k-1].revealed != true) {
            matrix[j+1][k-1].revealed = true;
            num_left--;
            if (matrix[j+1][k-1].adjacent == 0) {
                expandBlocks(j+1, k-1, max_rows, max_columns);
            }
        }

        // Center
        if (k > 0 && matrix[j+1][k].adjacent != -1 && matrix[j+1][k].revealed != true) {
            matrix[j+1][k].revealed = true;
            num_left--;
            if (matrix[j+1][k].adjacent == 0) {
                expandBlocks(j+1, k, max_rows, max_columns);
            }
        }

        // Right
        if (k < max_columns && matrix[j+1][k+1].adjacent != -1 && matrix[j+1][k+1].revealed != true) {
            matrix[j+1][k+1].revealed = true;
            num_left--;
            if (matrix[j+1][k+1].adjacent == 0) {
                expandBlocks(j+1, k+1, max_rows, max_columns);
            }
        }
    }
}

void MinesweeperQ::revealAll() {
    for (auto& row : matrix) {
        for (auto& col : row) {
            col.revealed = true;
        }
    }
    dataChanged(createIndex(0, 0), createIndex(getRows(), getColumns()), { RevealedType });
}

void MinesweeperQ::setRows(int count) {
    if (count > getRows()) {
        insertRows(0, count - getRows());
    } else if (count < getRows()) {
        removeRows(0, getRows() - count);
    }
    rowsChanged();
}

void MinesweeperQ::setColumns(int count) {
    if (count > getColumns()) {
        insertColumns(0, count - getColumns());
    } else if (count < getColumns()) {
        removeColumns(0, getColumns() - count);
    }
    columnsChanged();
}

// Inherited Read Methods
int MinesweeperQ::rowCount(const QModelIndex& /*parent*/) const {
    return matrix.size();
}

int MinesweeperQ::columnCount(const QModelIndex& /*parent*/) const {
    return matrix.at(0).size();
}

auto MinesweeperQ::data(const QModelIndex& index, int role) const
-> QVariant {
    QVariant result;
    switch (role) {
        case NumType : result = QVariant::fromValue(matrix.at(index.row()).at(index.column()).adjacent);
            break;
        case FlaggedType : result = QVariant::fromValue(matrix.at(index.row()).at(index.column()).flagged);
            break;
        case RevealedType : result = QVariant::fromValue(matrix.at(index.row()).at(index.column()).revealed);
            break;
        default : result = QVariant();
    }
    return result;
}

auto MinesweeperQ::headerData(int section, Qt::Orientation orientation, int role) const
-> QVariant {
    if (orientation == Qt::Horizontal)
        return QVariant();

    QVariant result;
    switch (role) {
        case Qt::DisplayRole : result = QString(section);
            break;
        default : result = QVariant();
    }
    return result;
}

auto MinesweeperQ::roleNames() const
-> QHash<int, QByteArray> {
    QHash<int, QByteArray> roles;
    roles[NumType] = "number";
    roles[FlaggedType] = "isFlagged";
    roles[RevealedType] = "isRevealed";
    return roles;
}

// Inherited Write Methods
bool MinesweeperQ::setData(const QModelIndex& index, const QVariant& value, int role) {
    bool result = false;
    switch (role) {
        case RevealedType : {
            result = true;
            matrix[index.row()][index.column()].revealed = qvariant_cast<bool>(value);
            num_left--;

            if (num_left == -1) {
                makeMinesweep(index.row(), index.column());
                break;
            }

            if (matrix[index.row()][index.column()].adjacent == 0) {
                expandBlocks(index.row(), index.column(), rowCount()-1, columnCount()-1);
                dataChanged(createIndex(0, 0), createIndex(getRows()-1, getColumns()-1), { RevealedType });
            } else if (matrix[index.row()][index.column()].adjacent == -1) {
                revealAll();
            } else {
                dataChanged(index, index, { RevealedType });
            }
            numLeftChanged();
        }
        break;
    case FlaggedType : {
        bool b = qvariant_cast<bool>(value);
        matrix[index.row()][index.column()].flagged = b;
        dataChanged(index, index, { FlaggedType });
        if (b == true) {
            num_flags++;
            numFlagsChanged();
        } else {
            num_flags--;
            numFlagsChanged();
        }
        result = true;
    }
        break;
    }

    return result;
}

auto MinesweeperQ::flags(const QModelIndex& /*index*/) const
-> Qt::ItemFlags {
    return Qt::ItemIsSelectable | Qt::ItemNeverHasChildren
            | Qt::ItemIsEnabled;
}

// Inherited Resize Methods
bool MinesweeperQ::insertRows(int before_row, int count, const QModelIndex& parent) {
    beginInsertRows(parent, before_row + 1, before_row + count);
    matrix.insert(matrix.begin() + before_row, count,
                  std::vector<block>(getColumns(), {0, false, false}));
    endInsertRows();
    return true;
}

bool MinesweeperQ::removeRows(int from_row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, from_row, from_row + count - 1);
    matrix.erase(matrix.begin() + from_row, matrix.begin() + from_row + count);
    endRemoveRows();
    return true;
}

bool MinesweeperQ::insertColumns(int before_column, int count, const QModelIndex& parent) {
    beginInsertColumns(parent, before_column + 1, before_column + count);
    for (auto& it : matrix) {
        it.insert(it.begin() + before_column, count, { 0, false, false });
    }
    endInsertColumns();
    return true;
}

bool MinesweeperQ::removeColumns(int from_column, int count, const QModelIndex &parent) {
    beginRemoveColumns(parent, from_column, from_column + count - 1);
    for (auto& it : matrix) {
        it.erase(it.begin() + from_column, it.begin() + from_column + count);
    }
    endInsertColumns();
    return true;
}

void MinesweeperQ::makeMinesweep(int rowSeed, int columnSeed) {
    num_left += getRows() * getColumns();
    numLeftChanged();

    int j, k;

    int max_rows = getRows() - 1;
    int max_columns = getColumns() - 1;
    for (int i = 0; i < num_bombs; i++) {
        do {
            j = getRandom(max_rows);
            k = getRandom(max_columns);
        } while(j == rowSeed || k == columnSeed || matrix[j][k].adjacent == -1);

        matrix[j][k].adjacent = -1;

        // Top row
        if (j > 0) {
            // Left
            if (k > 0 && matrix[j-1][k-1].adjacent != -1)
                matrix[j-1][k-1].adjacent++;

            // Center
            if (matrix[j-1][k].adjacent != -1)
                matrix[j-1][k].adjacent++;

            // Right
            if (k < max_columns && matrix[j-1][k+1].adjacent != -1)
                matrix[j-1][k+1].adjacent++;
        }

        // Middle Row
        if (k > 0 && matrix[j][k-1].adjacent != -1)
            matrix[j][k-1].adjacent++;

        if (k < max_columns && matrix[j][k+1].adjacent != -1)
            matrix[j][k+1].adjacent++;

        // Bottom Row
        if (j < max_rows) {
            // Left
            if (k > 0 && matrix[j+1][k-1].adjacent != -1)
                matrix[j+1][k-1].adjacent++;

            // Center
            if (matrix[j+1][k].adjacent != -1)
                matrix[j+1][k].adjacent++;

            // Right
            if (k < max_columns && matrix[j+1][k+1].adjacent != -1)
                matrix[j+1][k+1].adjacent++;
        }
    }

    expandBlocks(rowSeed, columnSeed, max_rows, max_columns);
    numLeftChanged();
    dataChanged(createIndex(0, 0), createIndex(max_rows, max_columns), { NumType, RevealedType });
}

void MinesweeperQ::reset() {
    num_left = 0;
    numLeftChanged();

    num_flags = 0;
    numFlagsChanged();

    for (auto& row : matrix) {
        for (auto& col : row) {
            col = { 0, false, false };
        }
    }
    dataChanged(createIndex(0, 0), createIndex(getRows(), getColumns()), {});
}
