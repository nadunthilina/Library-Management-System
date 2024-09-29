#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Book {
    int id;
    char title[100];
    char author[100];
    int available;
};

struct Member {
    int memberId;
    char name[50];
    int borrowedBookId;
};

int addBook();
int borrowBook();
int returnBook();
int viewBooks();
int viewMembers();
int addMember();
int deleteBook();
int deleteMember();

int main() {
    int choice;

    while (1) {
        printf("\n\n=== Library Management System ===\n");
        printf("1. Add Book\n");
        printf("2. Add Member\n");
        printf("3. View Books\n");
        printf("4. View Members\n");
        printf("5. Borrow Book\n");
        printf("6. Return Book\n");
        printf("7. Delete Book\n");
        printf("8. Delete Member\n");
        printf("0. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                addMember();
                break;
            case 3:
                viewBooks();
                break;
            case 4:
                viewMembers();
                break;
            case 5:
                borrowBook();
                break;
            case 6:
                returnBook();
                break;
            case 7:
                deleteBook();
                break;
            case 8:
                deleteMember();
                break;
            case 0:
                printf("Exiting...\n");
                exit(0);
                break;
            default:
                printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}

int addBook() {
    struct Book newBook;
    FILE *file = fopen("books.txt", "a");

    printf("Enter Book ID: ");
    scanf("%d", &newBook.id);
    printf("Enter Book Title: ");
    scanf(" %[^\n]%*c", newBook.title);
    printf("Enter Author: ");
    scanf(" %[^\n]%*c", newBook.author);
    newBook.available = 1;

    fwrite(&newBook, sizeof(struct Book), 1, file);
    fclose(file);

    printf("Book added successfully.\n");
    return 0;
}

int viewBooks() {
    struct Book book;
    FILE *file = fopen("books.txt", "r");

    if (file == NULL) {
        printf("No books found.\n");
        return -1;
    }

    printf("\n=== List of Books ===\n");
    while (fread(&book, sizeof(struct Book), 1, file)) {
        printf("ID: %d | Title: %s | Author: %s | Available: %d\n", book.id, book.title, book.author, book.available);
    }
    fclose(file);
    return 0;
}

int addMember() {
    struct Member newMember;
    FILE *file = fopen("members.txt", "a");

    printf("Enter Member ID: ");
    scanf("%d", &newMember.memberId);
    printf("Enter Member Name: ");
    scanf(" %[^\n]%*c", newMember.name);
    newMember.borrowedBookId = 0;

    fwrite(&newMember, sizeof(struct Member), 1, file);
    fclose(file);

    printf("Member added successfully.\n");
    return 0;
}

int viewMembers() {
    struct Member member;
    FILE *file = fopen("members.txt", "r");

    if (file == NULL) {
        printf("No members found.\n");
        return -1;
    }

    printf("\n=== List of Members ===\n");
    while (fread(&member, sizeof(struct Member), 1, file)) {
        printf("ID: %d | Name: %s | Borrowed Book ID: %d\n", member.memberId, member.name, member.borrowedBookId);
    }
    fclose(file);
    return 0;
}

int borrowBook() {
    int memberId, bookId;
    struct Book book;
    struct Member member;
    FILE *bookFile = fopen("books.txt", "r+");
    FILE *memberFile = fopen("members.txt", "r+");
    FILE *tempBook = fopen("tempBooks.txt", "w");
    FILE *tempMember = fopen("tempMembers.txt", "w");

    printf("Enter Member ID: ");
    scanf("%d", &memberId);
    printf("Enter Book ID to Borrow: ");
    scanf("%d", &bookId);

    int foundMember = 0, foundBook = 0;

    while (fread(&member, sizeof(struct Member), 1, memberFile)) {
        if (member.memberId == memberId) {
            foundMember = 1;
            if (member.borrowedBookId != 0) {
                printf("This member already borrowed a book.\n");
                return -1;
            }
            while (fread(&book, sizeof(struct Book), 1, bookFile)) {
                if (book.id == bookId && book.available == 1) {
                    foundBook = 1;
                    member.borrowedBookId = bookId;
                    book.available = 0;
                }
                fwrite(&book, sizeof(struct Book), 1, tempBook);
            }
        }
        fwrite(&member, sizeof(struct Member), 1, tempMember);
    }

    fclose(bookFile);
    fclose(memberFile);
    fclose(tempBook);
    fclose(tempMember);

    remove("books.txt");
    rename("tempBooks.txt", "books.txt");
    remove("members.txt");
    rename("tempMembers.txt", "members.txt");

    if (foundBook && foundMember) {
        printf("Book borrowed successfully.\n");
    } else if (!foundBook) {
        printf("Book not found or unavailable.\n");
    } else {
        printf("Member not found.\n");
    }

    return 0;
}

int returnBook() {
    int memberId, bookId;
    struct Book book;
    struct Member member;
    FILE *bookFile = fopen("books.txt", "r+");
    FILE *memberFile = fopen("members.txt", "r+");
    FILE *tempBook = fopen("tempBooks.txt", "w");
    FILE *tempMember = fopen("tempMembers.txt", "w");

    printf("Enter Member ID: ");
    scanf("%d", &memberId);

    int foundMember = 0, foundBook = 0;

    while (fread(&member, sizeof(struct Member), 1, memberFile)) {
        if (member.memberId == memberId && member.borrowedBookId != 0) {
            foundMember = 1;
            bookId = member.borrowedBookId;
            member.borrowedBookId = 0;

            while (fread(&book, sizeof(struct Book), 1, bookFile)) {
                if (book.id == bookId) {
                    foundBook = 1;
                    book.available = 1;
                }
                fwrite(&book, sizeof(struct Book), 1, tempBook);
            }
        }
        fwrite(&member, sizeof(struct Member), 1, tempMember);
    }

    fclose(bookFile);
    fclose(memberFile);
    fclose(tempBook);
    fclose(tempMember);

    remove("books.txt");
    rename("tempBooks.txt", "books.txt");
    remove("members.txt");
    rename("tempMembers.txt", "members.txt");

    if (foundBook && foundMember) {
        printf("Book returned successfully.\n");
    } else if (!foundBook) {
        printf("Book not found.\n");
    } else {
        printf("Member not found or no book borrowed.\n");
    }

    return 0;
}

int deleteBook() {
    int bookId;
    struct Book book;
    FILE *file = fopen("books.txt", "r");
    FILE *tempFile = fopen("tempBooks.txt", "w");

    printf("Enter Book ID to Delete: ");
    scanf("%d", &bookId);

    while (fread(&book, sizeof(struct Book), 1, file)) {
        if (book.id != bookId) {
            fwrite(&book, sizeof(struct Book), 1, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("books.txt");
    rename("tempBooks.txt", "books.txt");

    printf("Book deleted successfully.\n");
    return 0;
}

int deleteMember() {
    int memberId;
    struct Member member;
    FILE *file = fopen("members.txt", "r");
    FILE *tempFile = fopen("tempMembers.txt", "w");

    printf("Enter Member ID to Delete: ");
    scanf("%d", &memberId);

    while (fread(&member, sizeof(struct Member), 1, file)) {
        if (member.memberId != memberId) {
            fwrite(&member, sizeof(struct Member), 1, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("members.txt");
    rename("tempMembers.txt", "members.txt");

    printf("Member deleted successfully.\n");
    return 0;
}
