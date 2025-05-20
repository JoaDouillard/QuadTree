#pragma once
//Evidemment, il va falloir inclure les fichiers nécessaires pour que le code compile
#include <vector>

//Vous n'avez pas le droit de modifier cette partie du code jusqu'à la ligne notée par le commentaire //Vous pouvez modifier le code ci-dessous
#include <concepts>
#include <stdexcept>

/**
 * @brief Concept définissant les exigences pour les données pouvant être stockées dans un TQuadTree.
 *
 * Ce concept vérifie que le type T possède les méthodes x1(), y1(), x2() et y2() qui retournent des valeurs convertibles en float.
 * De plus, le type T doit être copiable et comparable pour l'égalité.
 *
 * @tparam T Le type des données à vérifier.
 *
 * Les exigences pour le type T sont les suivantes :
 * - T doit avoir une méthode x1() retournant un type convertible en float.
 * - T doit avoir une méthode y1() retournant un type convertible en float.
 * - T doit avoir une méthode x2() retournant un type convertible en float.
 * - T doit avoir une méthode y2() retournant un type convertible en float.
 * - T doit être copiable.
 * - T doit être comparable pour l'égalité.
 */
template<typename T>
concept QuadTreeData = requires(T t)
{
    { t.x1() } -> std::convertible_to<float>;
    { t.y1() } -> std::convertible_to<float>;
    { t.x2() } -> std::convertible_to<float>;
    { t.y2() } -> std::convertible_to<float>;
}&& std::copyable<T>&& std::equality_comparable<T>;

/**
 * @brief Structure définissant les limites d'une zone rectangulaire.
 */
struct SLimits
{
    float x1; ///< La coordonnée x du coin supérieur gauche.
    float y1; ///< La coordonnée y du coin supérieur gauche.
    float x2; ///< La coordonnée x du coin inférieur droit.
    float y2; ///< La coordonnée y du coin inférieur droit.

    bool operator==(const SLimits& other) const = default;
};

/**
 * @brief Classe de QuadTree.
 *
 * Cette classe permet de stocker des données de type T dans un QuadTree.
 *
 * @tparam T Le type des données à stocker.
 * T doit respecter le concept QuadTreeData.
 */
template <QuadTreeData T>
class TQuadTree {
    //Vous pouvez modifier le code ci-dessous
    //Attention à ne pas modifier les signatures des fonctions et des méthodes qui sont déjà présentes



public:
    using container = std::vector<T>;
    container _objects;
    SLimits m_limits;
    std::vector<std::shared_ptr<TQuadTree>> childs = { nullptr, nullptr ,nullptr ,nullptr };

    void subdivide() {
        float midX = (m_limits.x1 + m_limits.x2) / 2.0f;
        float midY = (m_limits.y1 + m_limits.y2) / 2.0f;

        childs[0] = std::make_unique<TQuadTree>(SLimits{ m_limits.x1, m_limits.y1, midX, midY });
        childs[1] = std::make_unique<TQuadTree>(SLimits{ midX, m_limits.y1, m_limits.x2, midY });
        childs[2] = std::make_unique<TQuadTree>(SLimits{ m_limits.x1, midY, midX, m_limits.y2 });
        childs[3] = std::make_unique<TQuadTree>(SLimits{ midX, midY, m_limits.x2, m_limits.y2 });
    }

    /**
      * @brief Itérateur pour parcourir les éléments du QuadTree.
      */
    class iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::input_iterator_tag;

    private:
        std::shared_ptr<container> _iteObjectsOne;
        size_t _index = 0;

    public:
        /**
         * @brief Constructeur par défaut de l'itérateur.
         *
         * Evidemment, il va falloir compléter cette fonction pour qu'elle initialise correctement l'itérateur.
         * Peut-être que l'itérateur devrait être initialisé à la fin de la liste de données.
         * Peut-etre qu'on peut ajouter d'autres constructeurs pour initialiser l'itérateur à un endroit spécifique.
         */
        iterator() = default;

        /**
         * @brief Constructeur avec paramètres
         */
        iterator(std::shared_ptr<container> iteObjects, size_t index)
            : _iteObjectsOne(iteObjects), _index(index) {
        }

        /**
         * @brief Opérateur de comparaison d'égalité.
         *
         * @returns true si les deux itérateurs sont à la même position.
         */
        bool operator==(const iterator& other) const
        {
            return _iteObjectsOne == other._iteObjectsOne;
        }

        /**
         * @brief Opérateur de pré-incrémentation de l'itérateur.
         *
         * @returns l'itérateur courant avancé vers la prochaine position valide.
         */
        iterator& operator++()
        {
            if (!_iteObjectsOne) {
                return *this;
            }
            _index++;
            if (_index >= _iteObjectsOne->size()) {
                _iteObjectsOne.reset();
                _index = 0;
            }
            return *this;
        }


        /**
         * @brief Opérateur de post-incrémentation de l'itérateur.
         *
         * @param int Un paramètre fictif pour distinguer l'opérateur de pré-incrémentation de celui-ci.
         *
         * @returns l'itérateur courant avant d'être avancé vers la prochaine position valide.
         */
        iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        /**
         * @brief Opérateur de déréférencement de l'itérateur.
         *
         * @returns une référence vers l'élément pointé par l'itérateur.
         */
        T& operator*() const
        {
            if (!_iteObjectsOne)
                throw std::out_of_range("Iterator out of bounds");
            return (*_iteObjectsOne)[_index];
        }

        /**
         * @brief Opérateur de déréférencement pour accès aux membres.
         *
         * @returns un pointeur vers l'élément pointé par l'itérateur.
         */
        T* operator->()
        {
            if (!_iteObjectsOne)
                throw std::out_of_range("Out Of Range");
            return &(*_iteObjectsOne)[_index];
        }
    };

    std::shared_ptr<container> _iteObjectsTwo;


public:
    /**
     * @brief Constructeur de la classe TQuadTree.
     *
     * @param limits Les limites géométriques du QuadTree.
     */
    TQuadTree(const SLimits& limits = { 0.0f,0.0f,1.0f,1.0f })
        : m_limits(limits)
    {
        //// Vérification que les limites sont valides                          // en fait ce n'est pas vraiment utile
        //if (limits.x1 >= limits.x2 || limits.y1 >= limits.y2) {
        //    throw std::invalid_argument("Invalid limits: x1 must be less than x2 and y1 must be less than y2");
        //}

        //// Initialisation des enfants à nullptr
        //for (int i = 0; i < 4; ++i) {
        //    m_children[i] = nullptr;
        //}

        //Evidemment, il va falloir compléter ce constructeur pour qu'il initialise correctement votre TQuadTree
    }

    TQuadTree(const TQuadTree& other)
        : m_limits(other.m_limits), _objects(other._objects)
    {
        //Evidemment, il va falloir compléter ce constructeur pour qu'il initialise correctement votre TQuadTree
        for (size_t i = 0; i < 4; ++i) {
            if (other.childs[i]) {
                childs[i] = std::make_shared<TQuadTree>(*other.childs[i]);
            }
            else {
                childs[i] = nullptr;
            }
        }
    }


    /**
     * @brief Retourne les limites géométriques de ce QuadTree
     */
    SLimits limits() const
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne les limites géométriques de ce QuadTree
        return m_limits;
    }

    /**
     * @brief Vérifie si le QuadTree est vide.
     *
     * Cette fonction vérifie si le QuadTree ne contient aucun élément de données, y compris dans la descendance.
     *
     * @return true si le QuadTree est vide, false sinon.
     */

    bool empty() const
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne true uniquement si le QuadTree est vide

        if (!_objects.empty()) {
            return false;
        }

        for (const auto& child : childs) {
            if (child) {
                return child->empty();
            }
        }

        return true;
    }

    /**
     * @brief Retourne la profondeur maximale du QuadTree.
     *
     * La profondeur maximale du QuadTree correspond nombre maximal de niveaux de descendants.
     */
    size_t depth() const
    {
        if (childs[0] == nullptr && childs[1] == nullptr && childs[2] == nullptr && childs[3] == nullptr) {
            return 1;
        }

        size_t maxDepth = 0;
        for (const auto& child : childs) {
            if (child && (child->_objects.size() > 0 || child->depth() > 1)) {
                maxDepth = std::max(maxDepth, child->depth());
            }
        }
        return maxDepth + 1;
    }

    /**
     * @brief Retourne le nombre d'éléments stockés dans le QuadTree.
     *
     * Cette fonction retourne le nombre d'éléments stockés dans le QuadTree en incluant toute la descendance.
     *
     * @return Le nombre d'éléments stockés dans le QuadTree.
     */

    size_t size() const {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne le nombre d'éléments stockés dans le QuadTree
        size_t taille = _objects.size();
        for (const auto& child : childs) {
            if (child) {
                taille += child->size();
            }
        }
        return taille;
    }

    /**
     * @brief Insère un élément dans le QuadTree.
     *
     * Cette fonction insère un élément de données dans le QuadTree.
     * Si l'élément est en dehors des limites du QuadTree, une exception de type std::domain_error est levée.
     * Si l'élément est dans les limites d'un enfant, il est inséré dans cet enfant.
     * Sinon, l'élément est ajouté à la liste des données du QuadTree.
     *
     * @param t L'élément à insérer dans le QuadTree.
     */
    void insert(const T& t) {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle insère l'élément dans le QuadTree

        if (!(t.x1() >= m_limits.x1 && t.x2() <= m_limits.x2 &&
            t.y1() >= m_limits.y1 && t.y2() <= m_limits.y2)) {
            throw std::domain_error("Items out of QuadTree");
        }

        if (!childs[0]) {
            subdivide();
        }

        bool inserted = false;

        for (auto& child : childs) {
            if (child && t.x1() >= child->m_limits.x1 && t.x1() <= child->m_limits.x2 &&
                t.x2() >= child->m_limits.x1 && t.x2() <= child->m_limits.x2 &&
                t.y1() >= child->m_limits.y1 && t.y1() <= child->m_limits.y2 &&
                t.y2() >= child->m_limits.y1 && t.y2() <= child->m_limits.y2) {
                child->insert(t);
                inserted = true;
                break;
            }
        }

        if (!inserted) {
            _objects.push_back(t);
        }
    }


    /**
     * @brief Vide le QuadTree.
     *
     * Cette fonction vide le QuadTree de toutes ses données.
     */
    void clear()
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle vide le QuadTree
        for (const auto& child : childs) {
            if (child) {
                child->clear();
            }
        }
        _objects.clear();
    }


    void remove(const T& t) {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retire l'élément du QuadTree
        auto it = std::find(_objects.begin(), _objects.end(), t);
        if (it != _objects.end()) {
            _objects.erase(it);
            return;
        }

        for (const auto& child : childs) {
            if (child) {
                child->remove(t);
            }
        }
    }


    /**
     * @brief Récupère tous les éléments stockés dans le QuadTree.
     *
     * Cette fonction retourne une liste de tous les éléments stockés dans le QuadTree,
     * y compris ceux stockés dans les enfants.
     *
     * @return Une liste de tous les éléments stockés dans le QuadTree.
     */
    container getAll() const
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne tous les éléments stockés dans le QuadTree
        container result = _objects;

        // Parcour des enfants
        for (const auto& child : childs) {
            if (child) {
                const container& childObjects = child->getAll();
                result.insert(result.end(), childObjects.begin(), childObjects.end());
            }
        }
        return result;
    }

    /**
     * @brief Trouve les éléments dans une zone spécifiée.
     *
     * Cette fonction recherche et retourne une liste de tous les éléments stockés dans le QuadTree
     * qui se trouvent totalement inclus dans la zone spécifiée par limits.
     *
     * @param limits Les limites de la zone de recherche.
     * @return Une liste de tous les éléments trouvés dans la zone spécifiée.
     */

    container findInscribed(const SLimits& limits) const
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne tous les éléments inclus dans la zone spécifiée

        container result;

        for (const auto& obj : _objects) {
            if (obj.x1() >= limits.x1 && obj.x2() <= limits.x2 &&
                obj.y1() >= limits.y1 && obj.y2() <= limits.y2) {
                result.push_back(obj);
            }
        }

        for (const auto& child : childs) {
            if (child) {
                container inscribedChilds = child->findInscribed(limits);
                result.insert(result.end(), inscribedChilds.begin(), inscribedChilds.end());
            }
        }

        return result;
    }
    /**
     * @brief Trouve les éléments dans une zone spécifiée.
     *
     * Cette fonction recherche et retourne une liste de tous les éléments stockés dans le QuadTree
     * qui sont en collision avec la zone spécifiée par limits.
     *
     * @param limits Les limites de la zone de recherche.
     * @return Une liste de tous les éléments trouvés dans la zone spécifiée.
     */
    container findColliding(const SLimits& limits) const
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne tous les éléments en collision avec la zone spécif
        container result;

        for (const auto& obj : _objects) {
            if (!(obj.x2() < limits.x1 || obj.x1() > limits.x2 || obj.y2() < limits.y1 || obj.y1() > limits.y2)) {
                result.push_back(obj);
            }
        }

        for (const auto& child : childs) 
        {
            if (child) {
                container collidingChilds = child->findColliding(limits);
                result.insert(result.end(), collidingChilds.begin(), collidingChilds.end());
            }
        }

        return result;
    }


    /**
     * @brief Retourne un iterateur permettant de lister un à un tous les éléments
     */
    iterator begin()
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne un iterateur de début
        _iteObjectsTwo = std::make_shared<container>(getAll());
        return iterator(_iteObjectsTwo, 0);

    }


    /**
     * @brief Retourne un iterateur permettant de lister un à un tous les éléments inclus dans les limites spécifiées
     *
     * Les éléments listés sont ceux qui entrent en collision avec les limites spécifiées
     */
    iterator beginColliding(const SLimits& limits)
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne un iterateur de début
        _iteObjectsTwo = std::make_shared<container>(findColliding(limits));
        return iterator(_iteObjectsTwo, 0);
    }

    /**
     * @brief Retourne un iterateur permettant de lister un à un tous les éléments inclus dans les limites spécifiées
     *
     * Les éléments listés sont ceux qui sont inclus dans les limites spécifiées
     */
    iterator beginInscribed(const SLimits& limits)
    {
        //Evidemment, il va falloir compléter cette fonction pour qu'elle retourne un iterateur de début
        _iteObjectsTwo = std::make_shared<container>(findInscribed(limits));
        return iterator(_iteObjectsTwo, 0);
    }

    /**
     * @brief Retourne un iterateur de fin
     */

    iterator end()
    {
        return {};  // black magic ???????????????????????
    }

};
